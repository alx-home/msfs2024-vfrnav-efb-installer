import { Dispatch, ReactElement, SetStateAction, StrictMode, useEffect, useState } from 'react'
import { createRoot } from 'react-dom/client'

import { App } from './App/App';

import './global.css';
import { Button } from '@Utils/Button';

const setPopupRef: {
  current?: Dispatch<SetStateAction<ReactElement | undefined>>
} = {};

window.pfatal = (message: string) => {
  setPopupRef.current?.(<div className='flex flex-col gap-y-6 grow'>
    <div className='text-3xl text-red-700'>Fatal Error !</div>
    <div className='text-xl gap-y-2'>
      {message.split('#').map(elem =>
        <div key={elem}>{elem}</div>
      )}
    </div>
    <div className='flex flex-row grow'>
      <Button active={true} onClick={() => setPopupRef.current?.(undefined)}>OK</Button>
    </div>
  </div>);
};

if (__WEB_BROWSER_TEST__) {
  window.findCommunity = () => {
    return Promise.resolve("Comunity_");
  }
  window.exists = () => {
    return Promise.resolve(true);
  }
  window.validate = () => {
  }
}

const Popup = () => {
  const [popup, setPopup] = useState<ReactElement | undefined>(undefined);

  useEffect(() => {
    setPopupRef.current = setPopup;
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
