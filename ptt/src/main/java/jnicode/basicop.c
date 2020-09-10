/******************************************************************************
模块名	： G729编解码器接口                       
文件名	：G729.C
相关文件	：
文件实现功能：数据转换
    作者	：chukaiyan
版本		：
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
2004-04-01	v1.0		chukaiyan	fixpoint						------------

******************************************************************************/

/*___________________________________________________________________________
 |                                                                           |
 | Basics operators.                                                         |
 |___________________________________________________________________________|
*/

/*___________________________________________________________________________
 |                                                                           |
 |   Include-Files                                                           |
 |___________________________________________________________________________|
*/
#include <stdio.h>
#include <stdlib.h>
#include<math.h>

#include "basicop.h"
/*___________________________________________________________________________
 |                                                                           |
 |   Local Functions                                                         |
 |___________________________________________________________________________|
*/
s16 sature(l32 L_var1);


/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/
l32 Overflow =0;
l32 Carry =0;


/*___________________________________________________________________________
 |                                                                           |
 |   Functions                                                               |
 |___________________________________________________________________________|
*/


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : sature                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Limit the 32 bit input to the range of a 16 bit word.                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 sature(l32 L_var1)
{
    s16 var_out;

    if (L_var1 > 0X00007fffL) {
        Overflow = 1;
        var_out = MAX_16;
    }
    else {
        if (L_var1 < (l32)0xffff8000L) {
            Overflow = 1;
            var_out = MIN_16;
        }
        else {
            Overflow = 0;
            var_out = extract_l(L_var1);
        }
    }
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : add                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the addition (var1+var2) with overflow control and saturation;|
 |    the 16 bit result is set at +32767 when overflow occurs or at -32768   |
 |    when underflow occurs.                                                 |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 add(s16 var1,s16 var2)
{
    s16 var_out;
    l32 L_somme;

    L_somme = (l32) var1 + (l32) var2;
    var_out = sature(L_somme);
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : sub                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the subtraction (var1+var2) with overflow control and satu-   |
 |    ration; the 16 bit result is set at +32767 when overflow occurs or at  |
 |    -32768 when underflow occurs.                                          |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 sub(s16 var1,s16 var2)
{
    s16 var_out;
    l32 L_diff;

    L_diff = (l32) var1 -  (l32) var2;
    var_out = sature(L_diff);
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : abs_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of var1; abs_s(-32768) = 32767.                         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 abs_s(s16 var1)
{
    s16 var_out;

    if (var1 == (s16)0X8000 ) {
        var_out = MAX_16;
    }
    else {
        if (var1 < 0) {
            var_out = -var1;
        }
        else {
            var_out = var1;
        }
    }

    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shl                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 left var2 positions.Zero fill|
 |   the var2 LSB of the result. If var2 is negative, arithmetically shift   |
 |   var1 right by -var2 with sign extension. Saturate the result in case of |
 |   underflows or overflows.                                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 shl(s16 var1,s16 var2)
{
    s16 var_out;
    l32 L_result;

    if (var2 < 0) {
        var_out = shr(var1,(s16) -var2);
    }
    else {
        L_result = (l32) var1 * ((l32) 1 << var2);
        if ( ((var2 > 15) && (var1 != 0)) ||
             (L_result != (l32)((s16) L_result)) ) {
            Overflow = 1;
            var_out = (var1 > 0) ? MAX_16 : MIN_16;
        }
        else {
            var_out = extract_l(L_result);
        }
    }

    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 right var2 positions with    |
 |   sign extension. If var2 is negative, arithmetically shift var1 left by  |
 |   -var2 with sign extension. Saturate the result in case of underflows or |
 |   overflows.                                                              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 shr(s16 var1,s16 var2)
{
    s16 var_out;

    if (var2 < 0) {
        var_out = shl(var1, (s16) -var2);
    }
    else {
        if (var2 >= 15) {
            var_out = (var1 < (s16)0) ? (s16)-1 : (s16)0;
        }
        else {
            if (var1 < 0) {
                var_out = ~(( ~var1) >> var2 );
            }
            else {
                var_out = var1 >> var2;
            }
        }
    }

    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult                                                    |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the multiplication of var1 by var2 and gives a 16 bit result  |
 |    which is scaled i.e.:                                                  |
 |             mult(var1,var2) = shr((var1 times var2),15) and               |
 |             mult(-32768,-32768) = 32767.                                  |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 mult(s16 var1, s16 var2)
{
    s16 var_out;
    l32 L_produit;

    L_produit = (l32)var1 * (l32)var2;

    L_produit = (L_produit & (l32) 0xffff8000L) >> 15;

    if (L_produit & (l32) 0x00010000L) {
        L_produit |= (l32) 0xffff0000L;
    }
    var_out = sature(L_produit);
    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mult                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   L_mult is the 32 bit result of the multiplication of var1 times var2    |
 |   with one shift left i.e.:                                               |
 |        L_mult(var1,var2) = shl((var1 times var2),1) and                   |
 |        L_mult(-32768,-32768) = 2147483647.                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_mult(//returnvalue: var1 * var2 * 2
			 s16	var1, //input: 16bits value
			 s16 var2 // input: 16bits value
			 )
{
    l32 L_var_out;

    L_var_out = (l32)var1 * (l32)var2;
    if (L_var_out != (l32)0x40000000L) 
	{
        L_var_out *= 2L;
    }
    else {
        Overflow = 1;
        L_var_out = MAX_32;
    }

    return(L_var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : negate                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate var1 with saturation, saturate in the case where input is -32768:|
 |                negate(var1) = sub(0,var1).                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 negate(s16 var1)
{
    s16 var_out;

    var_out = (var1 == MIN_16) ? MAX_16 : -var1;
    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : extract_h                                               |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Return the 16 MSB of L_var1.                                            |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 extract_h(l32 L_var1)
{
    s16 var_out;

    var_out = (s16) (L_var1 >> 16);
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : extract_l                                               |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Return the 16 LSB of L_var1.                                            |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 extract_l(l32 L_var1)
{
    s16 var_out;

    var_out = (s16) L_var1;
    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : round_linux                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Round the lower 16 bits of the 32 bit input number into its MS 16 bits  |
 |   with saturation. Shift the resulting bits right by 16 and return the 16 |
 |   bit number:                                                             |
 |               round_linux(L_var1) = extract_h(L_add(L_var1,32768))              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 round_linux(l32 L_var1)
{
    s16 var_out;
    l32 L_arrondi;

    L_arrondi = L_add(L_var1, (l32)0x00008000L);
    var_out = extract_h(L_arrondi);
    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mac                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation, return a 32 bit result:               |
 |        L_mac(L_var3,var1,var2) = L_add(L_var3,(L_mult(var1,var2)).        |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_mac(l32 L_var3, s16 var1, s16 var2)
{
    //static l32 L_var_out;
    static l32 L_produit;

    L_produit = L_mult(var1, var2);
    return L_add(L_var3, L_produit);
    //return(L_var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_msu                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result to L_var3 with saturation, return a 32 bit result:           |
 |        L_msu(L_var3,var1,var2) = L_sub(L_var3,(L_mult(var1,var2)).        |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_msu(l32 L_var3, s16 var1, s16 var2)
{
    l32 L_var_out;
    l32 L_produit;

    L_produit = L_mult(var1, var2);
    L_var_out = L_sub(L_var3, L_produit);
    return(L_var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_macNs                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 without saturation, return a 32 bit result. Generate   |
 |   carry and overflow values :                                             |
 |        L_macNs(L_var3,var1,var2) = L_add_c(L_var3,(L_mult(var1,var2)).    |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using op- |
 |    rators which take into account its value.                              |
 |___________________________________________________________________________|
*/

l32 L_macNs(l32 L_var3, s16 var1, s16 var2)
{
    l32 L_var_out;

    L_var_out = L_mult(var1, var2);
    L_var_out = L_add_c(L_var3, L_var_out);
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_msuNs                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result from L_var3 without saturation, return a 32 bit result. Ge-  |
 |   nerate carry and overflow values :                                      |
 |        L_msuNs(L_var3,var1,var2) = L_sub_c(L_var3,(L_mult(var1,var2)).    |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using op- |
 |    rators which take into account its value.                              |
 |___________________________________________________________________________|
*/

l32 L_msuNs(l32 L_var3, s16 var1, s16 var2)
{
    l32 L_var_out;

    L_var_out = L_mult(var1,var2);
    L_var_out = L_sub_c(L_var3,L_var_out);
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      |
 |   overflow control and saturation; the result is set at +214783647 when   |
 |   overflow occurs or at -214783648 when underflow occurs.                 |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_add(l32 L_var1, l32 L_var2)
{
    l32 L_var_out;

    L_var_out = L_var1 + L_var2;

    if (((L_var1 ^ L_var2) & MIN_32) == 0L) {
        if ((L_var_out ^ L_var1) & MIN_32) {
            L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
            Overflow = 1;
        }
    }
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   |
 |   overflow control and saturation; the result is set at +214783647 when   |
 |   overflow occurs or at -214783648 when underflow occurs.                 |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_sub(l32 L_var1, l32 L_var2)
{
    l32 L_var_out;

    L_var_out = L_var1 - L_var2;

    if (((L_var1 ^ L_var2) & MIN_32) != 0L) 
	{
        if ((L_var_out ^ L_var1) & MIN_32) 
		{
            L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
            Overflow = 1;
        }
    }
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add_c                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Performs 32 bits addition of the two 32 bits variables (L_var1+L_var2+C)|
 |   with carry. No saturation. Generate carry and Overflow values. The car- |
 |   ry and overflow values are binary variables which can be tested and as- |
 |   signed values.                                                          |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using op- |
 |    rators which take into account its value.                              |
 |___________________________________________________________________________|
*/
l32 L_add_c(l32 L_var1, l32 L_var2)
{
    l32 L_var_out;
    l32 L_test;
    l32 carry_int = 0;

    L_var_out = L_var1 + L_var2 + (l32)Carry;

    L_test = L_var1 + L_var2;

    if ((L_var1 > 0L) && (L_var2 > 0L) && (L_test < 0L)) {
        Overflow = 1;
        carry_int = 0;
    }
    else {
        if ((L_var1 < 0L) && (L_var2 < 0L) && (L_test > 0L)) {
            Overflow = 1;
            carry_int = 1;
        }
        else {
            if (((L_var1 ^ L_var2) < 0L) && (L_test > 0L)) {
                Overflow = 0;
                carry_int = 1;
            }
            else {
                Overflow = 0;
                carry_int = 0;
            }
        }
    }

    if (Carry) {
        if (L_test == MAX_32) {
            Overflow = 1;
            Carry = carry_int;
        }
        else {
            if (L_test == (l32) 0xFFFFFFFFL) {
                Carry = 1;
            }
            else {
                Carry = carry_int;
            }
        }
    }
    else {
        Carry = carry_int;
    }

    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub_c                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Performs 32 bits subtraction of the two 32 bits variables with carry    |
 |   (borrow) : L_var1-L_var2-C. No saturation. Generate carry and Overflow  |
 |   values. The carry and overflow values are binary variables which can    |
 |   be tested and assigned values.                                          |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using op- |
 |    rators which take into account its value.                              |
 |___________________________________________________________________________|
*/

l32 L_sub_c(l32 L_var1, l32 L_var2)
{
    l32 L_var_out;
    l32 L_test;
    l32 carry_int = 0;

    if (Carry) {
        Carry = 0;
        if (L_var2 != MIN_32) {
            L_var_out = L_add_c(L_var1,-L_var2);
        }
        else {
            L_var_out = L_var1 - L_var2;
            if (L_var1 > 0L) {
                Overflow = 1;
                Carry = 0;
            }
        }
    }
    else {
        L_var_out = L_var1 - L_var2 - (l32)0X00000001L;
        L_test = L_var1 - L_var2;

        if ((L_test < 0L) && (L_var1 > 0L) && (L_var2 < 0L)) {
            Overflow = 1;
            carry_int = 0;
        }
        else if ((L_test > 0L) && (L_var1 < 0L) && (L_var2 > 0L)) {
            Overflow = 1;
            carry_int = 1;
        }
        else if ((L_test > 0L) && ((L_var1 ^ L_var2) > 0L)) {
            Overflow = 0;
            carry_int = 1;
        }


        if (L_test == MIN_32) {
            Overflow = 1;
            Carry = carry_int;
        }
        else {
            Carry = carry_int;
        }
    }

    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_negate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate the 32 bit variable L_var1 with saturation; saturate in the case |
 |   where input is -2147483648 (0x8000 0000).                               |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_negate(l32 L_var1)
{
    l32 L_var_out;

    L_var_out = (L_var1 == MIN_32) ? MAX_32 : -L_var1;
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult_r                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as mult with rounding, i.e.:                                       |
 |     mult_r(var1,var2) = shr(((var1*var2) + 16384),15) and                 |
 |     mult_r(-32768,-32768) = 32767.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 mult_r(s16 var1, s16 var2)
{
    s16 var_out;
    l32 L_produit_arr;

    L_produit_arr = (l32)var1 * (l32)var2; /* product */
    L_produit_arr += (l32) 0x00004000L;        /* round_linux */
    L_produit_arr &= (l32) 0xffff8000L;
    L_produit_arr >>= 15;                        /* shift */

    if (L_produit_arr & (l32) 0x00010000L)  {
                                     /* sign extend when necessary */
        L_produit_arr |= (l32) 0xffff0000L;
    }

    var_out = sature(L_produit_arr);
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shl                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  |
 |   fill the var2 LSB of the result. If var2 is negative, L_var1 right by   |
 |   -var2 arithmetically shift with sign extension. Saturate the result in  |
 |   case of underflows or overflows.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

//该函数可以被优化，不要使用循环。 cky
l32 L_shl(l32 L_var1, s16 var2)
{
    l32 L_var_out = 0L;

    if (var2 <= (s16)0) 
	{

        L_var_out = L_shr(L_var1, (s16) -var2);
    }
    else 
	{
        for(; var2 > (s16)0; var2--)
		{
            if (L_var1 > (l32) 0X3fffffffL)
			{
                Overflow = 1;
                L_var_out = MAX_32;
                break;
            }
            else 
			{
                if (L_var1 < (l32) 0xc0000000L)
				{
                    Overflow = 1;
                    L_var_out = MIN_32;
                    break;
                }
            }
            L_var1 *= 2L;
            L_var_out = L_var1;
        }
    }
    return(L_var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shr                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
 |   sign extension. If var2 is negative, arithmetically shift L_var1 left   |
 |   by -var2 and zero fill the var2 LSB of the result. Saturate the result  |
 |   in case of underflows or overflows.                                     |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

l32 L_shr(l32 L_var1, s16 var2)
{
    l32 L_var_out;

    if (var2 < (s16)0)
	{
        L_var_out = L_shl(L_var1, (s16) -var2);
    }
    else 
	{
        if (var2 >= (s16)31) 
		{
            L_var_out = (L_var1 < 0L) ? -1L : 0L;
        }
        else 
		{
            if (L_var1< 0L)  
			{
                L_var_out = ~((~L_var1) >> var2);
            }
            else 
			{
                L_var_out = L_var1 >> var2;
            }
        }
    }
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as shr(var1,var2) but with rounding. Saturate the result in case of|
 |   underflows or overflows :                                               |
 |    If var2 is greater than zero :                                         |
 |       shr_r(var1,var2) = shr(add(var1,2**(var2-1)),var2)                  |
 |    If var2 is less than zero :                                            |
 |       shr_r(var1,var2) = shr(var1,var2).                                  |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

s16 shr_r(s16 var1, s16 var2)
{
    s16 var_out;

    if (var2 > (s16)15) {
        var_out = (s16)0;
    }
    else {
        var_out = shr(var1,var2);

        if (var2 > (s16)0) {
            if ((var1 & ((s16)1 << (var2-(s16)1))) != (s16)0) {
                var_out++;
            }
        }
    }
    return(var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mac_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation. Round the LS 16 bits of the result    |
 |   into the MS 16 bits with saturation and shift the result right by 16.   |
 |   Return a 16 bit result.                                                 |
 |            mac_r(L_var3,var1,var2) = round_linux(L_mac(Lvar3,var1,var2))        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/

s16 mac_r(l32 L_var3, s16 var1, s16 var2)
{
    s16 var_out;

    L_var3 = L_mac(L_var3, var1, var2);
    L_var3 = L_add(L_var3, (l32) 0x00008000L);
    var_out = extract_h(L_var3);
    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : msu_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result to L_var3 with saturation. Round the LS 16 bits of the res-  |
 |   ult into the MS 16 bits with saturation and shift the result right by   |
 |   16. Return a 16 bit result.                                             |
 |            msu_r(L_var3,var1,var2) = round_linux(L_msu(Lvar3,var1,var2))        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/

s16 msu_r(l32 L_var3, s16 var1, s16 var2)
{
    s16 var_out;

    L_var3 = L_msu(L_var3,var1,var2);
    L_var3 = L_add(L_var3, (l32) 0x00008000L);
    var_out = extract_h(L_var3);
    return(var_out);
}



/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_h                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 MS bits of the 32 bit output. The   |
 |   16 LS bits of the output are zeroed.                                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var_out <= 0x7fff 0000.                |
 |___________________________________________________________________________|
*/
l32 L_deposit_h(s16 var1)
{
    l32 L_var_out;
    L_var_out = (l32) var1 << 16;
    return(L_var_out);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_l                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 LS bits of the 32 bit output. The   |
 |   16 MS bits of the output are sign extended.                             |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0xFFFF 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

l32 L_deposit_l(s16 var1)
{
    l32 L_var_out;

    L_var_out = (l32) var1;
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shr_r                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as L_shr(L_var1,var2)but with rounding. Saturate the result in case|
 |   of underflows or overflows :                                            |
 |    If var2 is greater than zero :                                         |
 |       L_shr_r(var1,var2) = L_shr(L_add(L_var1,2**(var2-1)),var2)          |
 |    If var2 is less than zero :                                            |
 |       L_shr_r(var1,var2) = L_shr(L_var1,var2).                            |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/

l32 L_shr_r(l32 L_var1,s16 var2)
{
    l32 L_var_out;

    if (var2 > (s16)31) {
        L_var_out = 0L;
    }
    else {
        L_var_out = L_shr(L_var1,var2);
        if (var2 > (s16)0) {
            if ( (L_var1 & ( 1L << (s16)(var2-(s16)1) )) != 0L)  {
                L_var_out++;
            }
        }
    }
    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_abs                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of L_var1; Saturate in case where the input is          |
 |                                                               -214783648  |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x0000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/

l32 L_abs(l32 L_var1)
{
    l32 L_var_out;

    if (L_var1 == MIN_32) {
        L_var_out = MAX_32;
    }
    else {
        if (L_var1 < 0L) {
            L_var_out = -L_var1;
        }
        else {
            L_var_out = L_var1;
        }
    }

    return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sat                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    32 bit L_var1 is set to 2147833647 if an overflow occured or to        |
 |    -214783648 if an underflow occured on the most recent L_add_c, L_sub_c,|
 |    L_macNs or LmsuNs operations. The carry and overflow values are binary |
 |    values which can be tested and assigned values.                        |
 |                                                                           |
 |   Complexity weight : 4                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/

l32 L_sat (l32 L_var1)
{
    l32 L_var_out;


    L_var_out = L_var1;

    if (Overflow) {

        if (Carry) {
            L_var_out = MIN_32;
        }
        else {
            L_var_out = MAX_32;
        }

        Carry = 0;
        Overflow = 0;
    }

    return(L_var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_s                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shift needed to normalize the 16 bit varia- |
 |   ble var1 for positive values on the interval with minimum of 16384 and  |
 |   maximum of 32767, and for negative values on the interval with minimum  |
 |   of -32768 and maximum of -16384; in order to normalize the result, the  |
 |   following operation must be done :                                      |
 |                    norm_var1 = shl(var1,norm_s(var1)).                    |
 |                                                                           |
 |   Complexity weight : 15                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 000f.                |
 |___________________________________________________________________________|
*/

s16 norm_s(s16 var1)
{
    s16 var_out;

    if (var1 == (s16)0) {
        var_out = (s16)0;
    }
    else {
        if (var1 == (s16) 0xffff) {
            var_out = (s16)15;
        }
        else {
            if (var1 < (s16)0) {
                var1 = ~var1;
            }

            for(var_out = (s16)0; var1 < (s16)0x4000; var_out++) {
                var1 <<= 1;
            }
        }
    }

    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : div_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces a result which is the fractional integer division of var1 by  |
 |   var2; var1 and var2 must be positive and var2 must be greater or equal  |
 |   to var1; the result is positive (leading bit equal to 0) and truncated  |
 |   to 16 bits.                                                             |
 |   If var1 = var2 then div(var1,var2) = 32767.                             |
 |                                                                           |
 |   Complexity weight : 18                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 0000 <= var1 <= var2 and var2 != 0.            |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : var1 <= var2 <= 0x0000 7fff and var2 != 0.            |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
 |             It's a Q15 value (point between b15 and b14).                 |
 |___________________________________________________________________________|
*/

s16 div_s(s16 var1, s16 var2)
{
    s16 var_out = (s16)0;
    s16 iteration;
    l32 L_num;
    l32 L_denom;

    if ((var1 > var2) || (var1 < (s16)0) || (var2 < (s16)0)) 
	{
        printf("Division Error\n");
       // exit(0);
		return 0;
    }

    if (var2 == (s16)0) {
        printf("Division by 0, Fatal error \n");
        //exit(0);
		return MAX_16;
    }

    if (var1 == (s16)0) 
	{
        var_out = (s16)0;
    }
    else 
	{
        if (var1 == var2)
		{
            var_out = MAX_16;
        }
        else 
		{
            L_num = L_deposit_l(var1);
            L_denom = L_deposit_l(var2);

            for(iteration=(s16)0; iteration< (s16)15;iteration++) 
			{
                var_out <<=1;
                L_num <<= 1;

                if (L_num >= L_denom)
				{
                    L_num = L_sub(L_num,L_denom);
                    var_out = add(var_out, (s16)1);
                }
            }
        }
    }

    return(var_out);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_l                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shift needed to normalize the 32 bit varia- |
 |   ble l_var1 for positive values on the interval with minimum of          |
 |   1073741824 and maximum of 2147483647, and for negative values on the in-|
 |   terval with minimum of -2147483648 and maximum of -1073741824; in order |
 |   to normalize the result, the following operation must be done :         |
 |                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             |
 |                                                                           |
 |   Complexity weight : 30                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit s16 signed integer (s16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 001f.                |
 |___________________________________________________________________________|
*/

s16 norm_l(l32 L_var1)
{
    s16 var_out;

    if (L_var1 == 0L) 
	{
        var_out = (s16)0;
    }
    else 
	{
        if (L_var1 == (l32)0xffffffffL) 
		{
            var_out = (s16)31;
        }
        else 
		{
            if (L_var1 < 0L) 
			{
                L_var1 = ~L_var1;
            }

            for(var_out = (s16)0;L_var1 < (l32)0x40000000L;var_out++) 
			{
                L_var1 <<= 1L;
            }
        }
    }

    return(var_out);
}
/*
   Additional operators
*/
l32   L_mls( l32 Lv, s16 v )
{
   l32   Temp  ;

   Temp = Lv & (l32) 0x0000ffff ;
   Temp = Temp * (l32) v ;
   Temp = L_shr( Temp, (s16) 15 ) ;
   Temp = L_mac( Temp, v, extract_h(Lv) ) ;

   return Temp ;
}
/*__________________________________________________________________________
|                                                                           |
|   Function Name : div_l                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Produces a result which is the fractional integer division of L_var1 by|
|   var2; L_var1 and var2 must be positive and var2 << 16 must be greater or|
|   equal to L_var1; the result is positive (leading bit equal to 0) and    |
|   truncated to 16 bits.                                                   |
|   If L_var1 == var2 << 16 then div_l(L_var1,var2) = 32767.                |
|                                                                           |
|   Complexity weight : 20                                                  |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1                                                                 |
|             32 bit long signed integer (l32) whose value falls in the  |
|             range : 0x0000 0000 <= var1 <= (var2 << 16)  and var2 != 0.   |
|             L_var1 must be considered as a Q.31 value                     |
|                                                                           |
|    var2                                                                   |
|             16 bit s16 signed integer (s16) whose value falls in the |
|             range : var1 <= (var2<< 16) <= 0x7fff0000 and var2 != 0.      |
|             var2 must be considered as a Q.15 value                       |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    var_out                                                                |
|             16 bit s16 signed integer (s16) whose value falls in the |
|             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
|             It's a Q15 value (point between b15 and b14).                 |
|___________________________________________________________________________|
*/


s16 div_l( l32  L_num, s16 den ){

   s16   var_out = (s16)0;
   l32   L_den;
   s16   iteration;

   if ( den == (s16) 0 ) {
          printf("Division by 0 in div_l, Fatal error \n");
          exit(0);
      }

   if ( (L_num < (l32) 0) || (den < (s16) 0) ) {
          printf("Division Error in div_l, Fatal error \n");
          exit(0);
      }

   L_den = L_deposit_h( den ) ;

   if ( L_num >= L_den )
   {
      return MAX_16 ;
   }
    else {
        L_num = L_shr(L_num, (s16)1) ;
        L_den = L_shr(L_den, (s16)1);
        for(iteration=(s16)0; iteration< (s16)15;iteration++) {
            var_out = shl( var_out, (s16)1);
            L_num   = L_shl( L_num, (s16)1);
            if (L_num >= L_den) {
                L_num = L_sub(L_num,L_den);
                var_out = add(var_out, (s16)1);
            }
        }

        return var_out;
    }
}

/*---------------------------------------------------------------------------*
 *  Function  i_mult()                                                        *
 *  ~~~~~~~~~~~~~~~~~                                                        *
 * Integer multiplication.                                                   *
 *--------------------------------------------------------------------------*/
s16 i_mult(s16 a, s16 b)
{
   return  a*b;
}

/*====================================================================
	函数名	：inttofloat
	功能		：将Q型整数转换成浮点数
	算法实现	：
	引用全局变量：无
	输入参数说明：见下
	返回值说明：见下
====================================================================*/

d64 inttofloat( //output: 转换的浮点数
				 l32  r_iData, //input: Q型整数
				 l32  r_iQ //input: Q位
				 )
{
	d64 l_dQ = pow(2,r_iQ);

	d64 l_fData = r_iData;
	
	l_fData = l_fData / l_dQ;

	return l_fData;
}


/*====================================================================
	函数名	：inttofloat
	功能		：将Q型整数转换成浮点数
	算法实现	：
	引用全局变量：无
	输入参数说明：见下
	返回值说明：见下
====================================================================*/

l32 floattoint(//output: 转换后的Q型整数
				 d64  r_fData, //input: 浮点数
				 l32  r_iQ //input: Q位
				 )
{

	d64 l_iQ = pow(2,r_iQ);
	l32 l_iData = (l32)( r_fData * l_iQ + 0.5);
	return l_iData;
}

/*___________________________________________________________________________
 | Function Mpy_32_16()                                                      |
 |                                                                           |
 |   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      |
 |   by 2**15                                                                |
 |                                                                           |
 |   This operation can also be viewed as the multiplication of a Q31        |
 |   number by a Q15 number, the result is in Q31.                           |
 |                                                                           |
 |   L_32 = (hi1*n)<<1 + ((lo1*n)>>15)<<1                                |
 |                                                                           |
 | Arguments:                                                                |
 |                                                                           |
 |  hi          hi part of 32 bit number.                                    |
 |  lo          lo part of 32 bit number.                                    |
 |  n           16 bit number.                                               |
 |                                                                           |
 |___________________________________________________________________________|
*/

l32 Mpy_32_16(s16 hi, s16 lo, s16 n)
{
  l32 L_32;

  L_32 = L_mult(hi, n);
  L_32 = L_mac(L_32, mult(lo, n) , 1);

  return( L_32 );
}



/*___________________________________________________________________________
 |                                                                           |
 |  Function L_Extract()                                                     |
 |                                                                           |
 |  Extract from a 32 bit integer two 16 bit DPF.                            |
 |                                                                           |
 |  Arguments:                                                               |
 |                                                                           |
 |   L_32      : 32 bit integer.                                             |
 |               0x8000 0000 <= L_32 <= 0x7fff ffff.                         |
 |   hi        : b16 to b31 of L_32                                          |
 |   lo        : (L_32 - hi<<16)>>1                                          |
 |___________________________________________________________________________|
*/

void L_Extract(l32 L_32, s16 *hi, s16 *lo)
{
  *hi  = extract_h(L_32);
  *lo  = extract_l( L_msu( L_shr(L_32, 1) , *hi, 16384));  /* lo = L_32>>1   */
  return;
}

/*___________________________________________________________________________
 | Function Mpy_32()                                                         |
 |                                                                           |
 |   Multiply two 32 bit integers (DPF). The result is divided by 2**31      |
 |                                                                           |
 |   L_32 = (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1              |
 |                                                                           |
 |   This operation can also be viewed as the multiplication of two Q31      |
 |   number and the result is also in Q31.                                   |
 |                                                                           |
 | Arguments:                                                                |
 |                                                                           |
 |  hi1         hi part of first number                                      |
 |  lo1         lo part of first number                                      |
 |  hi2         hi part of second number                                     |
 |  lo2         lo part of second number                                     |
 |                                                                           |
 |___________________________________________________________________________|
*/

l32 Mpy_32(s16 hi1, s16 lo1, s16 hi2, s16 lo2)
{
  l32 L_32;

  L_32 = L_mult(hi1, hi2);
  L_32 = L_mac(L_32, mult(hi1, lo2) , 1);
  L_32 = L_mac(L_32, mult(lo1, hi2) , 1);

  return( L_32 );
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Div_32 taylor formula                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |             Fractional integer division of two 32 bit numbers.            |
 |             L_num / L_denom.                                              |
 |             L_num and L_denom must be positive and L_num < L_denom.       |
 |             L_denom = denom_hi<<16 + denom_lo<<1                          |
 |             denom_hi is a normalize number.                               |
 |             The result is in Q30.                                         |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_num                                                                  |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x0000 0000 < L_num < L_denom                         |
 |                                                                           |
 |    L_denom = denom_hi<<16 + denom_lo<<1      (DPF)                        |
 |                                                                           |
 |       denom_hi                                                            |
 |             16 bit positive normalized integer whose value falls in the   |
 |             range : 0x4000 < hi < 0x7fff                                  |
 |       denom_lo                                                            |
 |             16 bit positive integer whose value falls in the              |
 |             range : 0 < lo < 0x7fff                                       |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_div                                                                  |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  |
 |             It's a Q31 value                                              |
 |                                                                           |
 |  Algorithm:                                                               |
 |                                                                           |
 |  - find = 1/L_denom.                                                      |
 |      First approximation: approx = 1 / denom_hi                           |
 |      1/L_denom = approx * (2.0 - L_denom * approx )                       |
 |                                                                           |
 |  -  result = L_num * (1/L_denom)                                          |
 |___________________________________________________________________________|
*/

l32 Div_32(l32 L_num, s16 denom_hi, s16 denom_lo)
{
  s16 approx, hi, lo, n_hi, n_lo;
  l32 L_32;


  /* First approximation: 1 / L_denom = 1/denom_hi */

  approx = div_s( (s16)0x3fff, denom_hi);    /* result in Q14 */
                                                /* Note: 3fff = 0.5 in Q15 */

  /* 1/L_denom = approx * (2.0 - L_denom * approx) */

  L_32 = Mpy_32_16(denom_hi, denom_lo, approx); /* result in Q30 */


  L_32 = L_sub( (l32)0x7fffffffL, L_32);      /* result in Q30 */

  L_Extract(L_32, &hi, &lo);

  L_32 = Mpy_32_16(hi, lo, approx);             /* = 1/L_denom in Q29 */

  /* L_num * (1/L_denom) */

  L_Extract(L_32, &hi, &lo);
  L_Extract(L_num, &n_hi, &n_lo);
  L_32 = Mpy_32(n_hi, n_lo, hi, lo);            /* result in Q29   */
  L_32 = L_shl(L_32, 2);                        /* From Q29 to Q31 */

  return( L_32 );
}

/*___________________________________________________________________________
 |                                                                           |
 |  Function L_Comp()                                                        |
 |                                                                           |
 |  Compose from two 16 bit DPF a 32 bit integer.                            |
 |                                                                           |
 |     L_32 = hi<<16 + lo<<1                                                 |
 |                                                                           |
 |  Arguments:                                                               |
 |                                                                           |
 |   hi        msb                                                           |
 |   lo        lsf (with sign)                                               |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |             32 bit long signed integer (l32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   |
 |                                                                           |
 |___________________________________________________________________________|
*/

l32 L_Comp(s16 hi, s16 lo)
{
  l32 L_32;

  L_32 = L_deposit_h(hi);
  return( L_mac(L_32, lo, 1));          /* = hi<<16 + lo<<1 */
}

