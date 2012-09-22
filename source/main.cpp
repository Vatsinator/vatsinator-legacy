#include "vatsinatorapplication.h"

#ifdef Q_OS_DARWIN
#import <Foundation/Foundation.h>
#endif

int main(int argc, char** argv) {
  
#ifdef Q_OS_DARWIN
  NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
  [[NSFileManager defaultManager] changeCurrentDirectoryPath:resourcePath];
#endif
  
  return VatsinatorApplication(argc, argv).exec();
}
