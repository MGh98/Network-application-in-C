#ifndef structpartagees
#define structpartagees

typedef enum TypeOp {AJOUT, RETRAIT, SOLDE, NONE} TypeOp;

typedef struct {
	TypeOp op;
	char date[13];
	signed long montant; 

} Operation;

typedef struct {
	char* ID_compte;
	signed long Solde;
	Operation Op[10];

} Compte;

typedef struct {
   char* ID_client;
   char* password;
   unsigned int nb_compte;
   Compte* comptes;
   
} Client;

#endif 
