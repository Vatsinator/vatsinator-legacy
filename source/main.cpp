#include "vatsinatorapplication.h"

#ifdef Q_OS_DARWIN
#include "CoreFoundation/CoreFoundation.h"
#endif

int main(int argc, char** argv) {
  
#ifdef Q_OS_DARWIN
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  char path[PATH_MAX];
  if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        exit(1);
  }
  CFRelease(resourcesURL);
  
  chdir(path);
#endif
  
  return VatsinatorApplication(argc, argv).exec();
}
