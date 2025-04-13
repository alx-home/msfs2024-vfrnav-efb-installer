import { PropsWithChildren, useRef, useMemo, Children, isValidElement, useState, ReactElement, useCallback, KeyboardEvent, ReactNode, RefObject, FocusEvent, useEffect } from 'react';

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
   const arrowRef = useRef<HTMLButtonElement | null>(null);
   const style = useMemo(() => "bg-gray-700 shadow-md flex flex-col rounded-sm border-2 border-gray-900"
      + ((disabled ?? false) ? ' opacity-30' : ' group-hocus:bg-gray-800 group-hocus:drop-shadow-xl group-hocus:border-msfs group-has-[:focus]:border-msfs group-has-[:hover]:border-msfs cursor-pointer'), [disabled]);

   const childs = useMemo(() =>
      Children.toArray(children)
         .filter(child => isValidElement(child))
         .filter(child => child.type == Option<Id>) as ReactElement<OptionProps<Id>>[], [children]);

   const optionsRef = useMemo((): RefObject<HTMLButtonElement | null>[] =>
      childs.map(() => ({
         current: null
      }))
      , [childs]);

   const onBlur = useCallback((e: FocusEvent<HTMLButtonElement>) => {
      if (e.relatedTarget !== elemRef.current
         && e.relatedTarget !== arrowRef.current
         && !optionsRef.find(elem => elem.current === e.relatedTarget)
      ) {
         setOpen(false);
      }
   }, [optionsRef]);

   const preventDefault = useCallback((e: KeyboardEvent<HTMLButtonElement>) => {
      if (e.code == 'ArrowLeft' || e.code == 'ArrowRight' || e.code == 'ArrowDown' || e.code == 'ArrowUp') {
         e.stopPropagation()
         e.preventDefault()
      }
   }, []);

   const onKey = useCallback((e: KeyboardEvent<HTMLButtonElement>) => {
      preventDefault(e);

      if (e.code == 'ArrowLeft') {
         elemRef.current?.focus()
         setOpen(false);
      } else if (e.code == 'ArrowRight') {
         setOpen(true);
      } else if (e.code == 'ArrowDown') {
         setOpen(true);
         const index = optionsRef.findIndex(elem => document.activeElement === elem.current)
         if (index !== -1) {
            optionsRef[(index + 1) % optionsRef.length].current?.focus();
         } else if (open) {
            optionsRef[0].current?.focus();
         }
      } else if (e.code == 'ArrowUp') {
         setOpen(true);
         let index = optionsRef.findIndex(elem => document.activeElement === elem.current)
         if (index !== -1) {
            if (index == 0) {
               index = optionsRef.length - 1;
            } else {
               --index;
            }
            optionsRef[index].current?.focus();
         } else if (open) {
            optionsRef[optionsRef.length - 1].current?.focus();
         }
      }
   }, [open, optionsRef, preventDefault]);

   const options = useCallback((optionsRef?: RefObject<HTMLButtonElement | null>[]) => <div className={'flex flex-col p-2 ' + style + ' rounded-t-none border-t-0 '}>{
      childs.map((child, index) =>
         <button key={child.props.id as string}
            ref={optionsRef?.[index]}
            className='my-1 py-1 px-2 border-2 border-transparent hocus:border-msfs'
            onClick={() => {
               elemRef.current?.focus()
               onChange(child.props.id)
               setOpen(false)
            }}
            onMouseUp={() => {
               elemRef.current?.blur()
            }}
            onBlur={onBlur}
            onKeyDown={preventDefault}
            onKeyUp={onKey}
         >
            {child}
         </button>)
   }</div>, [style, childs, onBlur, preventDefault, onKey, onChange]);

   const labels = useMemo(() => childs.reduce((result, child) => {
      result.set(child.props.id, child.props.children);
      return result;
   }, new Map<Id, ReactNode>()), [childs]);

   const toggle = useCallback(() => {
      setOpen(open => !open)
   }, []);

   const focus = useCallback(() => {
      elemRef.current?.click()
   }, []);

   useEffect(() => {
      if (open) {
         optionsRef.find(elem => elem.current!.textContent === labels.get(value))?.current!.focus()
      }
   }, [labels, open, optionsRef, value]);

   return <div className={"group grow"}>
      <div className='flex flex-row'>
         <div className='flex flex-col grow [&>:first-child]:p-1 '>
            <button ref={elemRef}
               onClick={toggle}
               onBlur={onBlur}
               onKeyUp={onKey}
               onKeyDown={preventDefault}
               disabled={(disabled ?? false) || !(active ?? true)}
               className={'grow ' + style + ' border-r-0 rounded-r-none' + (open ? ' rounded-b-none' : '')}>
               <div className={'line-clamp-1 w-[100%] overflow-ellipsis text-xl font-semibold text-white ' + (className ?? "")} >
                  <div className='grow'>{labels.get(value)}</div>
               </div>
            </button>
            <div className='relative overflow-visible'>
               <div className='absolute'>
                  <div inert={!open} className={'overflow-hidden w-[calc(100%+22px)] duration-300 transition-opacity' + (open ? ' h-full' : ' max-h-0 opacity-0 pointer-events-none')}>
                     {options(optionsRef)}
                  </div>
               </div>
            </div>
            <div className={'block h-0 max-h-0 opacity-0 overflow-hidden'} inert={true}>
               {options()}
            </div>
         </div>
         <button
            tabIndex={-1}
            ref={arrowRef}
            onClick={focus}
            className={'border-l-0 rounded-l-none ' + style + (open ? ' rounded-b-none' : '')}>
            <img src={arrowImg} alt="arrow" width={20}
               className={'transition-all m-auto' +
                  (open ? ' -rotate-90' : '')} />
         </button>
      </div>
   </div>;
};