#ifndef __EG_AES_H__
#define __EG_AES_H__


#include "eg_porting_layer.h"


/***********************************************************************************************/

#define AES_ENCRYPT     1
#define AES_DECRYPT     0
typedef struct 
{   
	int nr;                     /*!<  number of rounds  */   
	unsigned int* rk;               /*!<  AES round keys    */    
	unsigned int buf[68];           /*!<  unaligned data    */
}aes_context;
void aes_setkey_enc( aes_context* ctx, const unsigned char* key, int keysize );
void aes_setkey_dec( aes_context* ctx, const unsigned char* key, int keysize );
void aes_crypt_ecb_update( aes_context* ctx, int mode, const unsigned char input[16], unsigned char output[16] );
unsigned char* aes_crypt_ecb( aes_context* ctx, int mode, const unsigned char* input, int slen, int* dlen );


/***********************************************************************************************/


int EG_Aes_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);
int EG_Aes_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);

int EG_Aes_udpBroadcast_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);
int EG_Aes_udpBroadcast_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);

int EG_Aes_lanMessageEncrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);
int EG_Aes_lanMessageDecrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);

void EG_hash_md5(uint8_t *input, int len, uint8_t *hash, int hlen);
void aes_result_dump(uint8_t *result, uint8_t length);

int  EG_Aes_init();
#endif 

