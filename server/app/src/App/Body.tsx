import { Button } from "@Utils/Button";
import { EndSlot, Input } from "@Utils/Input";
import { Scroll } from "@Utils/Scroll";
import { Option, Select } from "@Utils/Select";
import { PropsWithChildren, RefObject, SetStateAction, useCallback, useEffect, useMemo, useRef, useState } from "react";
import { addPopup, LoadingPopup, MessagePopup } from "./Popup";

const Elem = ({ children }: PropsWithChildren) => {
   return <div className='flex flex-row bg-slate-800 p-5 text-left'>
      {children}
   </div>
}

const Path = ({ onClick, path, defaultValue, placeholder, onChange, reload, validate: _validate, setIsValid, checkParent }: {
   onClick: (_value: string) => void,
   path: string,
   onChange: (_path: string) => void,
   defaultValue?: string,
   placeholder?: string,
   reload: boolean,
   validate?: (_: string) => void,
   setIsValid?: (_: boolean) => void,
   checkParent?: boolean
}) => {
   const ref = useRef<HTMLInputElement>(null);
   const callback = useCallback(() => onClick(ref.current!.value.length ? ref.current!.value : (defaultValue ?? path)), [onClick, defaultValue, path]);
   const validate = useCallback((path: string) => (
      checkParent ?
         window.parentExists(path) :
         window.exists(path))
      .then((value) => value ? (_validate?.(path) ?? true) : false
      ), [_validate, checkParent]);

   return <div className='flex flex-col grow justify-center'>
      <div className='flex flex-col shrink'>
         <Input active={true} value={path} ref={ref} defaultValue={defaultValue} placeholder={placeholder} onChange={onChange}
            validate={validate} reload={reload} setIsValid={setIsValid} className='peer'>
            <EndSlot>
               <div className='[&>*]:border-0 [&>*]:hover:border-l-2 [&>*]:bg-slate-600 [&>*]:hover:bg-slate-700 [&>*]:h-full'>
                  <Button active={true} className='px-4'
                     onClick={callback}>...</Button>
               </div>
            </EndSlot>
         </Input>
         <div className="hidden peer-[.invalid]:flex">
            <p className="pl-8 pt-1 text-red-500 text-base">
               Invalid path !
            </p>
         </div>
      </div>
   </div>
}


type StartupOption = 'Login' | 'Startup' | 'Never';

const useFolder = ({ placeholder, initPath, autoSub }: {
   placeholder?: string,
   initPath?: string,
   autoSub?: string
}
) => {
   const [path, setPath] = useState(initPath ?? '');
   const [pathValid, setPathValid] = useState(true);
   const [pathReload, setPathReload] = useState(false);
   const askPath = useCallback(async (value: string) => {
      const result = await window.openFolder((autoSub && value.lastIndexOf('\\') > 0) ? value.substring(0, value.lastIndexOf('\\') + 1) : value)
      if (result && result !== '') {
         setPathReload(true);
         setPath(autoSub ? result + '\\' + autoSub : result);
         setPathValid(true);
      }
   }, [autoSub]);

   useEffect(() => {
      if (pathReload) {
         setPathReload(false);
      }
   }, [pathReload]);

   return {
      path: path, valid: pathValid, setValid: setPathValid, ask: askPath, set: setPath, reload: pathReload, elem:
         <Path onClick={askPath} onChange={setPath} path={path} defaultValue='' reload={pathReload}
            placeholder={placeholder} setIsValid={setPathValid} checkParent={!!autoSub} />
   };
}

export const Body = ({ setCanContinue, validate }: {
   setCanContinue: (_setter: SetStateAction<boolean>) => void,
   validate: RefObject<(() => void) | null>
}) => {
   const { path: communityPath, valid: communityValid, set: setCommunityPath, elem: communityElem } = useFolder({ placeholder: 'Location of MSFS 2024 community folder', autoSub: 'alexhome-msfs2024-vfrnav' });
   const { path: installPath, valid: installPathValid, set: setInstallPath, elem: installPathElem } = useFolder({ placeholder: 'VFRNav\' Server installation path', autoSub: 'MSFS VFRNav Server' });

   const [startupOption, setStartupOption] = useState<StartupOption>('Login');

   const closeRef = useMemo<{
      current?: () => void
   }>(() => ({}), []);

   useEffect(() => {
      (async () => {
         const community = await window.findCommunity();
         if (community) {
            setCommunityPath(community);
         }
      })()
   }, [setCommunityPath])

   useEffect(() => {
      (async () => {
         const installPath = await window.defaultInstallPath();
         if (installPath) {
            setInstallPath(installPath);
         }
      })()
   }, [setInstallPath])

   useEffect(() => {
      setCanContinue(communityValid && installPathValid);
   }, [communityValid, installPathValid, setCanContinue]);

   useEffect(() => {
      validate.current = async () => {
         addPopup(<LoadingPopup title="Installing" message="Installing MSFS VFRNav server..." closeRef={closeRef} />)

         if (await window.validate(startupOption, communityPath, installPath)) {
            addPopup(<MessagePopup
               title={<div className='text-3xl text-blue-400'>Info</div>}
               message="MSFS VFRNav Successfully installed !"
               close={() => {
                  window.abort()
               }}
            />)
         }

         closeRef.current!();
      }
   }, [closeRef, communityPath, installPath, startupOption, validate])

   return <div className='flex flex-col grow min-h-0 justify-center overflow-hidden'>
      <div className='flex flex-col mx-auto min-w-[80%] overflow-hidden'>
         <Scroll className='flex-col'>
            <div className='flex flex-col grow p-5 gap-3 py-10'>
               <h2 className='mt-6 text-3xl'>Info</h2>
               <Elem>
                  <div className='flex flex-col gap-y-3'>
                     <p>
                        MSFS VFRNav&apos; can be used without this program by simply dragging the plugin into the community folder.<br />
                        Doing that does not allow to open PDF files from the computer nor accessing the UI via a Web brother.
                     </p>
                     <p>
                        This program aims at proxying file browsing requests to the os and serving the app with a web server.
                     </p>
                  </div>
               </Elem>
               <h2 className='mt-6 text-3xl'>Startup</h2>
               <Elem>
                  <div className='m-auto mr-5 grow min-w-0'>Auto start MSFS VFRNav&apos; server :</div>
                  <div className='shrink'>
                     <Select value={startupOption} active={true} onChange={setStartupOption} className='pl-3'>
                        <Option<StartupOption> id={'Login'}>Windows login</Option>
                        <Option<StartupOption> id={'Startup'}>MSFS startup</Option>
                        <Option<StartupOption> id={'Never'}>Never</Option>
                     </Select>
                  </div>
               </Elem>
               <h2 className='mt-6 text-3xl'>Install Path</h2>
               <Elem>
                  <div className='m-auto mr-5'>Community :</div>
                  {communityElem}
               </Elem>
               <Elem>
                  <div className='m-auto mr-5'>Destination :</div>
                  {installPathElem}
               </Elem>
            </div>
         </Scroll>
      </div>
   </div>
}