#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <functional>
using namespace std;


struct TProblem									//**Definition du probleme:
{
	std::string Nom;							//**Nom du fichier de donnees
	int NbVilles;								//**Taille du probleme: Nombre de villes
	std::vector<std::vector <int> > Distance;	//**Distance entre chaque paire de villes. NB: Tableau de 0 a NbVilles-1.  Indice 0 utilise.
};

struct TSolution								//**Definition d'une solution: 
{
	std::vector <int> Seq;				//**Sequence dans laquelle les villes sont visitees. NB: Tableau de 0 a NbVilles-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des distances incluant le retour au point initial
	bool Valide;						//**Etat de validite de la solution (presence de chacune des villes une et une seule fois)
};

struct TAlgo
{
	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines generees a chaque iteration
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la solution au depart de l'algorithme
	int		NbEvalBest;					//**Nombre d'evalutions de solutions necessaires pour trouver la meilleure solution de l'algorithme (solution finale ou BEST)... forme de convergence
};

#endif