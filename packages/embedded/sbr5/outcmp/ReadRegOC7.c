#include <outcompare.h>

#ifdef _OC7IF

/********************************************************************
*    Function Name:  ReadRegOC7		                            *
*    Description:    In Non PWM Mode, this function reads the OCRS  *
*                    reg or OCR reg based on input parameter        *
*    Parameters:     char reg                                       *
*    Return Value:   unsigned int OCRS (if reg is 0)                *
*                    or OCR (if reg is 1)                           *
********************************************************************/

unsigned int ReadRegOC7(char reg)
{   
    
    /* check if OC is in NON_PWM Mode */
    if((OC7CONbits.OCM & 0x06) != 0x06) 
    {                                                    
        if(reg)
        {       
            return OC7R; /* Output Compare main Register */
            
        }     
        return OC7RS;    /* Output Compare Secondary Register */
    }  
    return -1;  
}

#else
#warning "Does not build on this target"
#endif
