/* eslint-disable no-empty-pattern */
/* eslint-disable no-var */

declare var openFile: ({ }: {
   value: string,
   filters?: {
      name: string,
      value: string[]
   }[]
} | string) => Promise<string | undefined>;

declare var openFolder: (_val: string) => Promise<string | undefined>;
declare var findCommunity: () => Promise<string | undefined>;
declare var defaultInstallPath: () => Promise<string | undefined>;
declare var log: (_val: string) => void;
declare var exists: (_val: string) => Promise<boolean>;
declare var parentExists: (_val: string) => Promise<boolean>;
declare var abort: () => void;
declare var validate: (_startupOption: StartupOption, _communityPath: string, _installPath: string) => Promise<boolean>;
declare var display_fatal: (_message: string) => void;
declare var display_error: (_message: string) => void;
declare var display_warning: (_message: string) => void;
declare var display_info: (_message: string) => void;
declare const __WEB_BROWSER_TEST__: boolean;