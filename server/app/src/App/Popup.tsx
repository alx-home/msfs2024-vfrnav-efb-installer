import { Button } from "@Utils/Button";
import { Scroll } from "@Utils/Scroll";
import { Dispatch, ReactElement, SetStateAction, useEffect, useState } from "react";



let popupRef: Dispatch<SetStateAction<ReactElement | undefined>> | undefined = undefined;

let popupProm = Promise.resolve();

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const addPopup = (elem_: ReactElement<any>) => {
   popupProm = popupProm.then(() =>
      new Promise<void>((resolve) =>
         popupRef?.(<elem_.type {...elem_.props} close={elem_.props.close ? () => {
            elem_.props.close()
            resolve()
         } : resolve} />)
      )
   ).then(() => {
      popupRef?.(undefined)
   })
};

export const MessagePopup = ({ message, close, title }: {
   title: ReactElement
   message: string,
   close?: () => void
}) => {
   return <div className='flex flex-col gap-y-6 grow'>
      {title}
      <div className='text-xl gap-y-2 overflow-hidden'>
         <Scroll>
            <div className="mb-4" dangerouslySetInnerHTML={{ __html: message }}></div>
         </Scroll>
      </div>
      <div className='flex flex-row grow'>
         <Button active={true} onClick={close}>OK</Button>
      </div>
   </div>
};

export const LoadingPopup = ({ message, closeRef, close, title }: {
   title: string
   message: string,
   close?: () => void,
   closeRef: {
      current?: () => void
   }
}) => {
   useEffect(() => {
      if (close) {
         closeRef.current = close;
      }
   }, [close, closeRef])

   return <div className='flex flex-col gap-y-6 grow'>
      <div className='text-3xl text-blue-400'>{title}</div>
      <div className='text-xl gap-y-2 overflow-hidden'>
         <Scroll>
            <div className="mb-4" dangerouslySetInnerHTML={{ __html: message }}></div>
         </Scroll>
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

export const Popup = () => {
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