import { PropsWithChildren, useRef, useMemo, Children, isValidElement, useState, ReactElement, useCallback, KeyboardEvent, ReactNode } from 'react';

import arrowImg from '@images/arrow.svg';

type OptionProps<Id> = PropsWithChildren<{
   id: Id
}>;
export const Option = <Id,>({ children }: OptionProps<Id>) => {
   return children
}

export function Select<Id>({ children, className, active, disabled, value, onChange }: PropsWithChildren<{
   className?: string,
   active: boolean,
   disabled?: boolean,
   value: Id,
   onChange: (_value: Id) => void
}>) {
   const [open, setOpen] = useState(false);
   const elemRef = useRef<HTMLButtonElement | null>(null);
   const style = useMemo(() => "bg-gray-700 shadow-md flex flex-col rounded-sm border-2 border-gray-900"
      + ((disabled ?? false) ? ' opacity-30' : ' group-hocus:bg-gray-800 group-hocus:drop-shadow-xl group-hocus:border-msfs group-has-[:focus]:border-msfs group-has-[:hover]:border-msfs cursor-pointer'), [disabled]);

   const childs = useMemo(() =>
      Children.toArray(children)
         .filter(child => isValidElement(child))
         .filter(child => child.type == Option<Id>) as ReactElement<OptionProps<Id>>[], [children]);

   const options = useMemo(() => <div className={'flex flex-col p-2 ' + style + ' rounded-t-none border-t-0 '}>{
      childs.map(child =>
         <button key={child.props.id as string}
            className='border-2 border-transparent hocus:border-msfs p-2'
            onClick={() => {
               onChange(child.props.id)
               setOpen(false)
            }}
         >
            {child}
         </button>)
   }</div>, [childs, onChange, style]);

   const labels = useMemo(() => childs.reduce((result, child) => {
      result.set(child.props.id, child.props.children);
      return result;
   }, new Map<Id, ReactNode>()), [childs]);

   const onKey = useCallback((e: KeyboardEvent<HTMLButtonElement>) => {
      if (e.code == 'ArrowLeft') {
         setOpen(false);
      } else if (e.code == 'ArrowRight') {
         setOpen(true);
      }
   }, []);

   const toggle = useCallback(() => {
      elemRef.current?.blur();
      setOpen(open => !open);
   }, []);

   return <div className={"group grow"}>
      <div className='flex flex-row'>
         <div className='flex flex-col grow [&>:first-child]:p-1 '>
            <button ref={elemRef}
               onClick={toggle}
               onKeyUp={onKey}
               disabled={(disabled ?? false) || !(active ?? true)}
               className={'grow ' + style + ' border-r-0 rounded-r-none' + (open ? ' rounded-b-none' : '')}>
               <div className={'line-clamp-1 w-[100%] overflow-ellipsis text-xl font-semibold text-white ' + (className ?? "")} >
                  <div className='grow'>{labels.get(value)}</div>
               </div>
            </button>
            <div className='relative overflow-visible'>
               <div className='absolute'>
                  <div inert={!open} className={'overflow-hidden w-[calc(100%+22px)] duration-700 transition-[max-height]' + (open ? ' max-h-[500px]' : ' max-h-0 opacity-0 pointer-events-none')}>
                     {options}
                  </div>
               </div>
            </div>
            <div className={'block h-0 max-h-0 opacity-0 overflow-hidden'} inert={true}>
               {options}
            </div>
         </div>
         <button
            tabIndex={-1}
            onClick={toggle}
            className={'border-l-0 rounded-l-none ' + style + (open ? ' rounded-b-none' : '')}>
            <img src={arrowImg} alt="arrow" width={20}
               className={'transition-all ' +
                  (open ? ' -rotate-90' : '')} />
         </button>
      </div>
   </div>;
};