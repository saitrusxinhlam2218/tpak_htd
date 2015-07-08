#include <stdio.h>
/* #include "wgstrans.h" */
extern void  WGS84ToGoteborg(double inX,double inY, double *utX, double *utY);
extern void  GoteborgToWGS84(double inX,double inY, double *utX, double *utY);
extern void  WGS84ToST74(double inX,double inY, double *utX, double *utY);
extern void  ST74ToWGS84(double inX,double inY, double *utX, double *utY);


double round(double d) {
  return (double) (int) (d+0.5);
}

void p1(double inx, double iny)
{
  double outx, outy;
  WGS84ToST74(inx, iny, &outx, &outy);
/*  outx = round(outx);
 *  outy = round(outy);
 */
  printf("\t%f\t%f\t%13.6f\t%13.6f\n", inx, iny, outx, outy);
/*
  printf("\nWGS-84\t%f\t%f\n", inx, iny);
  printf("ST-74\t%-9.0f\t%-9.0f\n\n", outx, outy);
 */
}

void main (int argc, char **argv)
{
  int i = 1;
  int res;
  double d, inx, iny, outx, outy;

  if (argc != 3) {
    if (argc == 2)
      printf("usage: %s xkoord ykoord\n",*argv);
    p1(60.185244, 19.125582);
    p1(58.771796, 17.227708);
    return;
  }
  
  res = sscanf((char *) argv[1], "%lf", &inx);
  res = sscanf((char *) argv[2], "%lf", &iny);
/*
  while (++i <= argc) {
    ++argv;
    res = sscanf(*argv, "%lf", &d);
    printf("arg:%d:\targv:%s:\tres:%d\td:%f\n", i, *argv, res, d);
  }
*/
  p1(inx, iny);
}
