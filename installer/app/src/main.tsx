import { Dispatch, ReactElement, SetStateAction, StrictMode, useEffect, useState } from 'react'
import { createRoot } from 'react-dom/client'

import { App } from './App/App';

import './global.css';
import { Button } from '@Utils/Button';

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

const Fatal = ({ message, close }: {
  message: string,
  close?: () => void
}) => {
  return <div className='flex flex-col gap-y-6 grow'>
    <div className='text-3xl text-red-700'>Fatal Error !</div>
    <div className='text-xl gap-y-2'>
      <div dangerouslySetInnerHTML={{ __html: message }}></div>
    </div>
    <div className='flex flex-row grow'>
      <Button active={true} onClick={close}>OK</Button>
    </div>
  </div>
}

const Error = ({ message, close }: {
  message: string,
  close?: () => void
}) => {
  return <div className='flex flex-col gap-y-6 grow'>
    <div className='text-3xl text-red-700'>Error !</div>
    <div className='text-xl gap-y-2'>
      <div dangerouslySetInnerHTML={{ __html: message }}></div>
    </div>
    <div className='flex flex-row grow'>
      <Button active={true} onClick={close}>OK</Button>
    </div>
  </div>
}

const Warning = ({ message, close }: {
  message: string,
  close?: () => void
}) => {
  return <div className='flex flex-col gap-y-6 grow'>
    <div className='text-3xl text-yellow-500'>Warning !</div>
    <div className='text-xl gap-y-2'>
      <div dangerouslySetInnerHTML={{ __html: message }}></div>
    </div>
    <div className='flex flex-row grow'>
      <Button active={true} onClick={close}>OK</Button>
    </div>
  </div>
}

window.display_warning = (message: string) => {
  addPopup(<Warning message={message} />)
};

window.display_error = (message: string) => {
  addPopup(<Error message={message} />)
};

window.display_fatal = (message: string) => {
  addPopup(<Fatal message={message} />)
};

const Popup = () => {
  const [popup, setPopup] = useState<ReactElement | undefined>(undefined);

  useEffect(() => {
    popupRef = setPopup;
    return () => popupRef = undefined
  }, []);

  return <div className={'fixed flex flex-col w-full h-full bg-opacity-80 bg-slate-600 z-50 justify-center'
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

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <Popup />
    <App />
  </StrictMode>
)
