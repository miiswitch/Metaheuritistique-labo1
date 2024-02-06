#include "Entete.h"
#pragma comment (lib,"DescenteDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (DescenteDLL.dll et DescenteDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur. 
//Indiquer les arguments du programme dans les proprietes du projet - debogage - arguements.
//Sinon, utiliser le repertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure TProblem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem & unProb, TAlgo & unAlgo);

//DESCRIPTION:	Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'ecran pour validation
extern "C" _declspec(dllimport) void AfficherSolution(const TSolution uneSolution, TProblem unProb, std::string Titre);

//DESCRIPTION:	Evaluation de la fonction objectif d'une solution et MAJ du compteur d'evaluation. La fonction objectif represente la distance totale de la tournee incluant le retour un point initial.
//				NB: L'evaluation d'une solution inclue la validation de la presence de chacune des villes une et une seule fois
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution & uneSol, TProblem unProb, TAlgo & unAlgo);

//DESCRIPTION:	Creation d'une sequence (permutation) aleatoire de villes et appel a l'evaluation de la fonction objectif.
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution & uneSolution, TProblem unProb, TAlgo & unAlgo);

//DESCRIPTION: Affichage a l'ecran de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier(en append) de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultatsFichier(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
extern "C" _declspec(dllimport) void	LibererMemoireFinPgm(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

TSolution Appliquer2opt(TSolution uneSol, size_t a, size_t b);
//... vous pouvez ajouter vos fonctions locales

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char* Param[])
{
	TSolution Courante;		//Solution active au cours des iterations
	TSolution Next;			//Solution voisine retenue a une iteration
	TSolution Best;			//Meilleure solution depuis le début de l'algorithme	//Non utilisee pour le moment 
	TProblem LeProb;		//Definition de l'instance de probleme
	TAlgo LAlgo;			//Definition des parametres de l'agorithme
	string NomFichier;

	//**Lecture des parametres
	NomFichier.assign(Param[1]);
	LAlgo.TailleVoisinage = atoi(Param[2]);
	LAlgo.NB_EVAL_MAX = atoi(Param[3]);

	srand((unsigned)time(NULL));		//**Precise un germe pour le generateur aleatoire

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);

	//**Creation de la solution initiale 
	CreerSolutionAleatoire(Courante, LeProb, LAlgo);
	AfficherSolution(Courante, LeProb, "SOLUTION INITIALE: ");
	//**Enregistrement des infos concernant la meilleure solution
	LAlgo.FctObjSolDepart = Courante.FctObj;
	LAlgo.NbEvalBest = LAlgo.CptEval;

	do
	{
		Next = GetSolutionVoisine(Courante, LeProb, LAlgo);
		//AfficherSolution(Courante, LeProb, "COURANTE: ");
		//AfficherSolution(Next, LeProb, "NEXT: ");
		if (Next.FctObj <= Courante.FctObj)	//**amelioration ou egalite
		{
			//Si amelioration stricte: Affichage et conservation du moment ou celle-ci est trouvee (apres combien d'evaluations de solutions)
			if (Next.FctObj < Courante.FctObj)
			{
				cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Next.FctObj << endl;
				//AfficherSolution(Next, LeProb, "NOUVELLE COURANTE: ");
				LAlgo.NbEvalBest = LAlgo.CptEval;
			}
			//Modification de la solution courante
			Courante = Next;
		}
	} while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj != 0); //Critere d'arret (ne pas enlever/modifier)

	AfficherResultats(Courante, LeProb, LAlgo);
	AfficherResultatsFichier(Courante, LeProb, LAlgo, "Resultats.txt");

	LibererMemoireFinPgm(Courante, Next, Best, LeProb);

	system("PAUSE");

	return 0;
}

//DESCRIPTION: Creation d'une solution voisine a partir de la solution courante (uneSol) qui ne doit pas etre modifiee.
//Dans cette fonction, appel de la fonction AppliquerVoisinage() pour obtenir une solution voisine selon un TYPE DE VOISINAGE selectionne + Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//Ainsi, si la ReGLE DE PIVOT necessite l'etude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage()" sera appelee plusieurs fois.
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	/*
		Structure : 2-opt
		Parcours : partiellement orienté
		Règle de pivot : k-improve (best)
	*/

	TSolution unVoisin = uneSol; // On initialise unVoisin avec la solution courante

	int k = unAlgo.TailleVoisinage; // Nombre de voisins à générer

	// On génère k voisins et on conserve le meilleur
	for (int i = 0; i < k; i++)
	{
		const TSolution unAutreVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);
	
		if (unAutreVoisin.FctObj < unVoisin.FctObj)
			unVoisin = unAutreVoisin; // On conserve le meilleur voisin parmis les k voisins
	};
	return unVoisin;
}

//DESCRIPTION: Fonction appliquant le type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB: La solution courante (uneSol) ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	TSolution Copie = uneSol;

	//Transformation de la solution Copie selon le type (structure) de voisinage selectionne : 2-opt
	// - Stratégie d'orientation partiellement orienté pour le voisinage : 
	// Les arêtes sont constitués de deux villes consécutives dans la séquence à la position tel que les arêtes sont [i, i+1] et [j, j+1]

	const size_t seqSize = Copie.Seq.size();
	const size_t i = rand() % seqSize; // Position aléatoire de la première arête dans la séquence en respectant la contrainte

	// On prend dans unProb les k villes les plus proche de la ville i et i+1, en faisant attention à ne pas prendre une ville à côté de celles ci
	constexpr int k = 5;

	size_t idVillesProches[k];

	// Liste des distances de la ville i + distances de la ville i+1
	std::vector<int> distancesVilles;
	distancesVilles.reserve(seqSize);

	for (size_t j = 0; j < seqSize; j++)
	{
		distancesVilles.push_back(unProb.Distance[i][j] + unProb.Distance[(i+1) % seqSize][(j+1) % seqSize]);
	}

	// Retire les villes i, i-1 and i+1 pour éviter leur sélection
	for (int offset = -1; offset <= 1; offset++)
	{
		int a = (i + offset) % seqSize; // Utilisation de modulo pour éviter le débordement
		distancesVilles[a] = INT_MAX;
	}

	// On prend les k villes les plus proches de la ville i
	for (size_t a = 0; a < k; a++)
	{
		auto min = std::min_element(distancesVilles.begin(), distancesVilles.end());
		idVillesProches[a] = std::distance(distancesVilles.begin(), min);
		distancesVilles[a] = INT_MAX;
	}

	// Choisi aléatoirement une ville parmis les k villes les plus proches de la ville i
	const size_t j = idVillesProches[rand() % k];

	Copie = Appliquer2opt(Copie, (i + 1) % seqSize, j);

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);

	return (Copie);
}


TSolution Appliquer2opt(TSolution uneSol, size_t a, size_t b)
{
    int taille = uneSol.Seq.size();
	if (a > b)
	{
		auto tmp = a;
		a = (b + 1) % taille;
		b = (tmp - 1 + taille) % taille;
	}
    while (a < b)
    {
        auto tmp = uneSol.Seq[a];
        uneSol.Seq[a] = uneSol.Seq[b];
        uneSol.Seq[b] = tmp;
        a = (a + 1) % taille;
        b = (b - 1 + taille) % taille;
    }
    return uneSol;
}