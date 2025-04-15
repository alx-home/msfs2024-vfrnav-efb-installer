import { UserConfig, build, createServer } from "vite";
import eslint from 'vite-plugin-eslint';
import react from '@vitejs/plugin-react'
import cssInjectedByJsPlugin from 'vite-plugin-css-injected-by-js';
import path from "path";
import { fileURLToPath } from "url";
import tailwindcss from "tailwindcss";
import autoprefixer from "autoprefixer";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const minify = true;
const webBrowserTest = process.env.WEB_BROWSER_TEST;

const Config: UserConfig = {
   mode: process.env.BUILD_TYPE,
   define: {
      __WEB_BROWSER_TEST__: webBrowserTest
   },
   build: {
      lib: false,
      minify: minify,
      rollupOptions: {
         output: {
            manualChunks: undefined
         },
      },
      outDir: "../../build/installer/app",
      ssr: false,
      sourcemap: process.env.BUILD_TYPE === 'development',
      emptyOutDir: true,
   },
   resolve: {
      alias: {
         "@Events": path.resolve(__dirname, "./src/Events"),
         "@fonts": path.resolve(__dirname, "./src/fonts"),
         "@images": path.resolve(__dirname, "./src/images"),
         "@app": path.resolve(__dirname, "./src/app"),
         "@Ol": path.resolve(__dirname, "./src/Ol"),
         "@pages": path.resolve(__dirname, "./src/pages"),
         "@polyfills": path.resolve(__dirname, "./src/polyfills"),
         "@public": path.resolve(__dirname, "./public"),
         "@Settings": path.resolve(__dirname, "./src/Settings"),
         "@shared": path.resolve(__dirname, "./shared"),
         "@Utils": path.resolve(__dirname, "./src/Utils"),
      },
      extensions: [
         '.js',
         '.json',
         '.jsx',
         '.ts',
         '.tsx',
      ],
   },
   plugins: [
      react(),
      cssInjectedByJsPlugin(),
      eslint(),
   ],
   css: {
      postcss: {
         plugins: [
            autoprefixer,
            tailwindcss
         ]
      },
   }
};

if (process.env.BUILD_TYPE === 'production') {
   await build(Config);
} else {
   await createServer({
      ...Config,
      server: {
         port: 4000,
         host: 'localhost',
      }
   }).then((server) => server.listen(4000));

}