
#include "Main.h"

Promise<>
Main::abort() {
   co_return webview_.terminate();
}