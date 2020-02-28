#include "common.h"
#include <sys/prctl.h>

namespace Common {

void set_thread_name(const char* name) {
  prctl(PR_SET_NAME, name, 0, 0, 0);
}

}
