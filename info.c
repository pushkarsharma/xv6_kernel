#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  //Just making a sytem call while passing the arguments
  int param = atoi(argv[1]);
  info(param);
  exit();
}
