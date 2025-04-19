import { Dispatch, PropsWithChildren, ReactElement, RefObject, SetStateAction, useCallback, useEffect, useRef, useState } from 'react'

import logo from '@images/app-icon.svg';
import { EndSlot, Input } from '@Utils/Input';
import { Button } from '@Utils/Button';
import { Scroll } from '@Utils/Scroll';
import { Option, Select } from '@Utils/Select';

const Header = () => {
  return <div className='flex flex-row shrink bg-msfs p-5 shadow-md [&>*]:drop-shadow-xl'>
    <img src={logo} alt='logo' className='h-10'></img>
    <h1 className='text-3xl ml-2'>MSFS VFRNav&apos; Server Installer</h1>
  </div>
}

const Trailer = ({ canContinue, validate }: {
  canContinue: boolean,
  validate: () => void
}) => {
  return <div className='flex flex-row shrink bg-slate-800 p-2 shadow-md justify-end'>
    <div className='flex flex-row shrink'>
      <Button className='px-4' active={canContinue} disabled={!canContinue} onClick={validate}>Continue</Button>
      <div className='[&>*]:bg-red-800 [&>*]:hover:bg-red-500 [&>*]:hover:border-white'>
        <Button className='px-4' active={true} onClick={() => { window.abort() }}>Abort</Button>
      </div>
    </div>
  </div>
}

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
  const validate = useCallback((path: string) => (checkParent ? window.parentExists(path) : window.exists(path)).then((value) => value ? (_validate?.(path) ?? true) : false
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

const Body = ({ setCanContinue, validate }: {
  setCanContinue: (_setter: SetStateAction<boolean>) => void,
  validate: RefObject<(() => void) | null>
}) => {
  const { path: communityPath, valid: communityValid, set: setCommunityPath, elem: communityElem } = useFolder({ placeholder: 'Location of MSFS 2024 community folder', autoSub: 'alexhome-msfs2024-vfrnav' });
  const { path: installPath, valid: installPathValid, set: setInstallPath, elem: installPathElem } = useFolder({ placeholder: 'VFRNav\' Server installation path', autoSub: 'MSFS VFRNav Server' });

  const [startupOption, setStartupOption] = useState<StartupOption>('Login');

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
    validate.current = () => {
      window.validate(startupOption, communityPath, installPath);
    }
  })

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


let popupRef: Dispatch<SetStateAction<ReactElement | undefined>> | undefined = undefined;

let popupProm = Promise.resolve();

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const addPopup = (elem_: ReactElement<any>) => {
  return popupProm = popupProm.then(() =>
    new Promise<void>((resolve) =>
      popupRef?.(<elem_.type {...elem_.props} close={resolve} />)
    ).then(() => popupRef?.(undefined))
  )
};

const MessagePopup = ({ message, close, title }: {
  title: ReactElement
  message: string,
  close?: () => void
}) => {
  return <div className='flex flex-col gap-y-6 grow'>
    {title}
    <div className='text-xl gap-y-2 overflow-hidden'>
      <Scroll>
        <div dangerouslySetInnerHTML={{ __html: message }}></div>
      </Scroll>
    </div>
    <div className='flex flex-row grow'>
      <Button active={true} onClick={close}>OK</Button>
    </div>
  </div>
};

window.display_warning = (message: string) => {
  addPopup(<MessagePopup
    title={<div className='text-3xl text-yellow-500'>Warning !</div>}
    message={message}
  />)
};

window.display_error = (message: string) => {
  addPopup(<MessagePopup
    title={<div className='text-3xl text-red-700'>Error !</div>}
    message={message}
  />)
};

window.display_fatal = (message: string) => {
  addPopup(<MessagePopup
    title={<div className='text-3xl text-red-700'>Fatal Error !</div>}
    message={message}
  />)
};

window.display_info = (message: string) => {
  addPopup(<MessagePopup
    title={<div className='text-3xl text-blue-400'>Info</div>}
    message={message}
  />)
};

const Popup = () => {
  const [popup, setPopup] = useState<ReactElement | undefined>(undefined);

  useEffect(() => {
    popupRef = setPopup;
    return () => popupRef = undefined
  }, []);

  return <div className={'absolute flex flex-col w-full h-full bg-opacity-80 bg-slate-600 z-50 justify-center'
    + (popup === undefined ? ' hidden' : '')
  }>
    <div className='flex flex-row box-border relative m-auto w-full max-w-4xl max-h-full'>
      <div className='flex flex-row grow bg-menu border-2 hover:border-msfs px-8 py-5 shadow-slate-950 shadow-md m-8 max-h-full'>
        <div className='flex flex-row grow overflow-hidden'>
          {popup ?? <></>}
        </div>
      </div>
    </div>
  </div>
}

export const App = () => {
  const [canContinue, setCanContinue] = useState(false);

  const validateRef = useRef<() => void>(null)

  const validate = useCallback(() => {
    validateRef.current?.();
  }, []);

  return <div className='flex flex-col h-full text-xl '>
    <Header />
    <div className='relative flex flex-col h-full w-full overflow-hidden'>
      <Popup />
      <Body setCanContinue={setCanContinue} validate={validateRef} />
      <Trailer canContinue={canContinue} validate={validate} />
    </div>
  </div>
}