import { useCallback, useRef, useState } from 'react'

import logo from '@images/app-icon.svg';
import { Button } from '@Utils/Button';
import { Popup } from './Popup';
import { Body } from './Body';

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