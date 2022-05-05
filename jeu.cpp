#include <sstream>
#include "jeu.h"
#include <vector>
#include <string>
using namespace std;

void modele::Jeu::demarrer()
{
    string entree;
    int count = 1;
    bool couleur = getTour(count);

    while (true)
    {
       afficherEchiquier();

       string str1;
       stringstream str2(entree);

       vector<string> vec1;

       while (str2 >> str1)
       {
           vec1.push_back(str1);
       }

       int colonne = vec1[0][0] - 'a';
       int ligne = 8 - (vec1[0][1] - '0');
       pair<int,int> init = make_pair(colonne, ligne);

       colonne = vec1[1][0] - 'a';
       ligne = 8  - (vec1[1][1] - '0');
       pair<int,int> finale = make_pair(colonne, ligne);

       if (echequier_.getPiece(init) == nullptr)
       {
           continue;
       }
       if (echequier_.getPiece(init)->getCouleur() != couleur)
       {
           continue;
       }

       if (echequier_.getPiece(init)->getTypePiece() == ROI && echequier_.distance(init, finale)> 1)
       {

               count++;
               couleur = getTour(count);


           continue;
       }

       if (echequier_.mouvementPiece(init, finale))
       {
           if (estEnEchec(couleur))
           {
              continue;
           }
           else
           {
               count++;
               couleur = getTour(count);
           }
       }
    }

}
string modele::Jeu::afficherCouleur(bool couleur)
{
    if (couleur)
    {
        return "White";
    }
        return "Black";
}
bool modele::Jeu::estEnEchec(bool couleur) const
{
    bool couleurAdverse = true;
    if (couleur)
    {
        couleurAdverse = false;
    }

    pair<int,int> coordoneeRoi = echequier_.getRoi(couleur);
    vector<pair<int,int>> coordoneePiece = echequier_.getCoordoneePiece(couleurAdverse);

    for (auto adversite : coordoneePiece)
    {
        if (echequier_.mouvementLegal(adversite, coordoneeRoi))
        {
            return true;
        }
    }
    return false;
}
bool modele::Jeu::estEnEchecEtMat(bool couleur)
{
    if (estEnEchec(couleur))
    {
        vector<pair<int,int>> coordoneePiece = echequier_.getCoordoneePiece(couleur);
        vector<pair<int,int>> coordonee = echequier_.getCoordonee();

        for (auto &cPiece : coordoneePiece)
        {
            for (auto &c : coordonee)
            {
                if (echequier_.mouvementPiece(cPiece, c))
                {
                    if (estEnEchec(couleur) == false)
                    {
                        return false;
                    }
                }
            }
        }

    }
    else
    {
        return false;
    }
    return true;
}
bool modele::Jeu::estMatchNul(bool couleur){
    if (estEnEchec(couleur))
    {
        return false;
    }
    else
        {
            vector<pair<int,int>> coordoneePiece = echequier_.getCoordoneePiece(couleur);
            vector<pair<int,int>> coordonee = echequier_.getCoordonee();

            for (auto &cPiece : coordoneePiece)
            {
                for (auto &c : coordonee)
                {
                    if (echequier_.mouvementPiece(cPiece, c))
                    {

                        if (estEnEchec(couleur) == false)
                        {

                            return false;
                        }
                    }
                }
            }
        }
        return true;
}
bool modele::Jeu::getTour(int i) const
{
    if (i % 2 == 1)
    {
        return true;
    }
    return false;
}
void modele::Jeu::switchGuiTurn()
{
   tour_ = !tour_;
}
void modele::Jeu::refaireMouvement()
{
    mouvement1_ = "";
    mouvement2_ = "";
}
bool modele::Jeu::bouger(std::pair<int,int> coordoneeInit, std::pair<int,int> coordoneeFinale)
{
    return echequier_.mouvementPiece(coordoneeInit, coordoneeFinale);
}
void modele::Jeu::afficherEchiquier()
{
    echequier_.afficher();
}

void modele::Jeu::getEntree(QString entree)
{
    //qDebug() << "Game saw that " << input << "was clicked, and will now respond.";

    // If this is the first click, store it in move1
    if (mouvement1_ == "")
    {
        mouvement1_ = entree.toStdString();
    }
    // If this is the second click, store it in move2
    else
    {
        mouvement2_ = entree.toStdString();


        // We can now pass the move to the Game
        int colonne = mouvement1_[0] - 'a';
        int ligne = 8 - (mouvement1_[1] - '0');
        pair<int, int> init = make_pair(ligne,colonne);

        colonne = mouvement2_[0] - 'a';
        ligne = 8 - (mouvement2_[1] - '0');

        pair<int, int> finale = make_pair(ligne,colonne);		// convert substring to pair

        // Verify that a piece was selected
        if (echequier_.getPiece(init) == nullptr)
        {
            envoyerSignal("Invalid Move");
            refaireMouvement();
            return;
        }

        if (echequier_.getPiece(init)->getCouleur() != tour_)
        {
            std::cout << "Error: It's " << afficherCouleur(tour_) << "'s turn." << '\n';

            emit envoyerSignal("Invalid Move");
            refaireMouvement();
            return;
        }

        // Check if this is an attempted castle and handle accordingly
        if (echequier_.getPiece(init)->getTypePiece() == ROI && echequier_.distance(init, finale) > 1)
        {

        }

        // Attempt to move piece
        // TODO: Check for castling here?
        else if (echequier_.bougerPiece(init, finale))
        {
            // Verify that move doesn't put player in check, else switch players
            if (estEnEchec(tour_))
            {
                // If move puts player in check, print error, revert move, and let player enter different move
                std::cout << "Error: This leaves " << afficherCouleur(tour_) << " in check.\n";
                //qDebug() << "Error: This leaves you in check.";
                //board.revertLastMove();
                envoyerSignal("Invalid Move");
            }
            // If the move was valid, switch turns and send "Valid" response
            else
            {
                switchGuiTurn();

                // Send QString response containing the two spaces of the valid move
                QString sendStr = "";
                QString part1 = QString::fromStdString(mouvement1_);
                QString part2 = QString::fromStdString(mouvement2_);
                sendStr += part1;
                sendStr += part2;
                envoyerSignal(sendStr);
            }
        }
        else
        {
            std::cout << "Error: Invalid move.\n";
           // qDebug() << "Error: Invalid move.";
            envoyerSignal("Invalid Move");
        }

        if(estEnEchecEtMat(tour_)==true)
        {
            envoyerSignal("Checkmate");
        }
        else if (estEnEchec(tour_)==true)
        {
            envoyerSignal("Check");
        }

        refaireMouvement();
    }

}

