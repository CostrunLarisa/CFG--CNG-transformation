#include <iostream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include <algorithm>
using namespace std;


class CFG
{   static int contor;
    map<string,string > simplificari;
    map<string,string> pas6;
   static string lambda;            //avem variabila lambda care va fi vizibila pentru toate obiectele clasei
                                    //chiar daca nu se modifica
    set<string> terminale;          //retinem  literele care apartin lui T
    set<string> neterminale;
    set<string> Inac;            //vector in care punem productiile accesibile; productiile care nu apartin setului sunt inaccesibile
    set<string> Neutil;          //vector in care punem productiile utilizabile;  productiile care nu apartin setului sunt neutilizabile
    map< string,set<string> > productii;   //multimea de productii
public:
    CFG(){  productii["S"].insert("");}
    CFG(set<string> T,set<string> N,map< string,set<string> > prod)
    {
        this->terminale=T;
        this->neterminale=N;
        this->productii=prod;
    }
    set<string>getInacc(){return this->Inac;}
    map<string,string> getPas(){return this->pas6;}
    set<string> getNeutil(){return this->Neutil;}
     map<string,string > getSimplificari(){return this->simplificari;}
     map< string,set<string> > getProductii (){return this->productii;};
    void stepOne();             //elimin productiile neutilizabile sau inaccesibile;
    void stepTwo();
    void stepThree();
    void config();
    void stepFive();
    void stepSix();
    string inlocuieste(string,string,int);
    set<string> cautaReden();
    int detLungime(string);
    bool toDel(string);
    bool apare(string);
    set<string> generareProductii(string,set<string>,vector<string>);
    map <string,set<string> > deleteLambda(string);
    void setInaccesibile();
    void setNeutilizabile();
    set<string> getProd(string);
    bool isReden(string ,set<string> );
    bool isUtilizabil(string);         //metoda care verifica daca o productie este accesibila
    friend istream& operator>>(istream&,CFG&);
    friend ostream& operator<<(ostream&,const CFG&);
};
string CFG::lambda="lambda";
int CFG::contor=0;
bool CFG::isUtilizabil(string ch)
{
    for(int i=0;i<ch.length();i++)
        if(toupper(ch[i])==ch[i])
        {   string s(1,ch[i]);
             if(Neutil.find(s) ==Neutil.end())return 0;
        }
    return 1;
}

void CFG::setNeutilizabile()
{
    int ok;
    for(auto prod:productii)
    { ok=1;
        for(auto elem2:prod.second)
        {

            for(int i=0;i<elem2.length();i++)
            {if(toupper(elem2[i])==elem2[i])     //daca productia are un terminal
                {
                    ok=0;
                    break;
                }
            }

         if(ok==1)Neutil.insert(prod.first);                          //daca am gasit macar un terminal atunci adaug productia
        }
    }
    for(auto prod:productii)                   //in caz contrar verific daca productia are neterminalele care deja sunt in multime
    {ok=1;
        for(auto elem2:prod.second)
        {
            for(int i=0;i<elem2.length();i++)
            {if(toupper(elem2[i])==elem2[i])     //daca productia are un terminal
                {

                    if(isUtilizabil(elem2))
                    {ok=0;
                        Neutil.insert(prod.first);
                        break;
                    }
                }
            }
            if(ok==0)break;
        }
    }

}

void CFG::setInaccesibile()
{
    Inac.insert("S");
    for(auto elem2:productii["S"])
    {
        for(int i=0;i<elem2.length();i++)
            {if(toupper(elem2[i])==elem2[i])     //luam productiile care se afle in membrul drept
                {
                    string s(1,elem2[i]);
                   Inac.insert(s);
                }
            }
    }
    for(auto elem:Inac)
    {
        for(auto el:productii[elem])
            for(int i=0;i<el.length();i++)
                {if(toupper(el[i])==el[i])             //daca e neterminal si apare in productie,il adaugam multimii
                    {  string s(1,el[i]);
                        Inac.insert(s);
                    }
                }
    }
}
void CFG::stepOne()
{   setNeutilizabile();
    for(auto prod:productii)
    {  int ok;
        if(Neutil.size())                       //daca se afla in multimea neutilizabilelor atunci e ok,nu modific
        {ok=1;

                                                //daca nu se afla atunci trebuie sa sterg productia,si o sterg
                                                //si acolo unde apare un membrul drept al altei productii
                                                //aplic rationamentul si pentru inaccesibile
            for(auto elem:Neutil)
            {
                if(elem==prod.first)
                {
                    ok=0;
                    break;
                }
            }
        }
        if(ok==1)                               //sterg productia
        {
            for(auto elem:productii)
                for(auto elem2:elem.second)
                    for(int i=0;i<elem2.length();i++)
                        {   string s(1,elem2[i]);
                            if(s==prod.first )
                            {   string s=elem2;
                                s.erase(s.begin()+i);
                                this->productii[elem.first].erase(elem2);
                                this->productii[elem.first].insert(s);
                            }
                        }
        productii.erase(prod.first);
        neterminale.erase(prod.first);
        }
    }
    setInaccesibile();
    for(auto prod:productii)
    { if(Inac.size())
        {
            if(Inac.find(prod.first)==Inac.end())                               //sterg productia
            {
                for(auto elem:productii)
                {   if(elem.first!=prod.first)
                    for(auto elem2:elem.second)
                    {  for(int i=0;i<elem2.length();i++)
                        {   string s(1,elem2[i]);
                            if(s==prod.first)
                            {  string s=elem2;
                                s.erase(s.begin()+i);
                                this->productii[elem.first].erase(elem2);
                                this->productii[elem.first].insert(s);
                            }
                        }

                    }
                }
                productii.erase(prod.first);
                neterminale.erase(prod.first);
            }
        }else break;
    }

   Inac.clear();            //golesc multimea inaccesibilelor si neutilizabilelor
   Neutil.clear();

}
map <string,set<string> > CFG::deleteLambda(string ch)
{
    for(auto elem:productii)
        if(elem.first!=ch)
       { for(auto elem2:elem.second)
        {
            for(int i=0;i<elem2.length();i++)
            {   string s(1,elem2[i]);
                if(s==ch && elem2.length()==1)    //membrul drept era format doar din ce trebuie sa stergem,atunci
                                                                            //punem lambda
                {
                    elem2=lambda;
                }
                else if(s==ch && elem2.length()>1)
                { //in caz contrar doar stergem vechea productie
                string s=elem2;
                s.erase(s.begin()+i);
                this->productii[elem.first].erase(elem2);
                this->productii[elem.first].insert(s);
                }
            }
        }
        }
    return productii;
}
set<string> CFG::generareProductii(string ch,set<string> Inserari,vector<string> Eliminate)
//in aceasta metoda mi se genereaza toate productiile posibile in momentul in care elimin mai multe lambde productii
//la final mi se returneaza un set cu aceste productii pe care le voi insera in productia curenta
{int ok;
for(int i=0;i<ch.length();i++)
{ string s(1,ch[i]);ok=1;
    if(find(Eliminate.begin(),Eliminate.end(),s)!=Eliminate.end())
       {
           string n=ch;ok=0;
           n.erase(n.begin()+i);
           Inserari.insert(n);

          Inserari=generareProductii(n,Inserari,Eliminate);
       }

}

Inserari.insert(ch);
return Inserari;
}
void CFG::stepTwo()
{
    vector<string> contor;                      //vector in care vad cate neterminale cu "lambda-sters" are productia
    vector<string> prodEliminate;               //neterminalele pentru care am sters lambda-productia
    set<string> combinatiiInserate;          //toate combinatiile pe care trebuie sa le adaugam unei productii care le contine
    for(auto elem:productii)
        for(auto elem2:elem.second)
        {
            if(elem2==lambda)
             {   if(elem.second.size()==1)   //daca neterminalul are o lambda productie si este singura productie,il elimin
                    {
                    this->productii=deleteLambda(elem.first); //stergem neterminalul din toate productiile
                    this->productii.erase(elem.first);
                    this->neterminale.erase(elem.first);

                    }
                if(elem.second.size()>1)    //daca neterminalul are mai multe productii
                {
                    prodEliminate.push_back(elem.first);
                    this->productii[elem.first].erase(elem2); //ii stergem doar lambda productia
                }
             }
        }

      if(prodEliminate.size())      //daca am gasit mai multe neterminale cu lambda-productii
    {
        for(auto elem:productii)
        {
            for(auto elem2:elem.second)
            {int pozitie;
                 for(int i=0;i<elem2.length();i++)
                    {   string s(1,elem2[i]);
                        if(find(prodEliminate.begin(),prodEliminate.end(),s)!=prodEliminate.end())
                            {
                                contor.push_back(s);
                                pozitie=i;
                            }
                    }

                if (contor.size()==1)           //daca in productie am gasit doar un neterminal
                {
                    string net=elem2;
                    net.erase(net.begin()+pozitie);
                    combinatiiInserate.insert(net);
                }
                if(contor.size()>1)
                {
                    combinatiiInserate=generareProductii(elem2,combinatiiInserate,prodEliminate);
                } contor.clear();
            }

            for(auto it:combinatiiInserate)
            {
                productii[elem.first].insert(it);
            }
            combinatiiInserate.clear();

        }
    }

}
bool CFG::apare(string ch)
{
    for(auto elem:productii)
    {  if(elem.first!=ch)
        for(auto elem2:elem.second)
        {
            if(elem2==ch)return 1;
        }
    }
    return 0;
}
set<string> CFG::cautaReden()
{   int ok;
    set<string>rezultat;
    for(auto elem:productii)
    {   ok=1;
        for(auto elem2:elem.second)
        {
            for(int it=0;it<elem2.length();it++)
            {
                string s(1,elem2[it]);
                if(terminale.find(s)==terminale.end())
                {ok=0;break;}            //daca este un neterminal atunci nu reprezinta o redenumire
            }
        }
        if(ok==1 && apare(elem.first))rezultat.insert(elem.first);
    }
    return rezultat;
}
set<string> CFG::getProd(string ch) //metoda acre returneaza toate productiile unui neterminal
{
    set<string> membru;
    for(auto elem:productii[ch])
                membru.insert(elem);
    return membru;
}
bool CFG::isReden(string ch,set<string> reden)  //metoda care verifica daca un neterminal reprezinta o redenumire
{
    return(reden.find(ch)!=reden.end());
}
bool CFG::toDel(string ch)
{
    for(auto elem:productii)
    {if(elem.first!=ch)
        for(auto elem2:elem.second)
        {
            for(int it=0;it<elem2.length();it++)
            {
                string s(1,elem2[it]);
                if(s==ch)return 0;
            }
        }
    }
    return 1;
}
void CFG::stepThree()
{
    set<string>  Redenumiri; // neterminalele pe care le gasim
    Redenumiri=cautaReden();
    set<string> netermin;
    for(auto elem:productii)
    {
        for(auto elem2:elem.second)

            if(elem2.length()==1 && isReden(elem2,Redenumiri))      //daca avem o productie formata doar dintr-un neterminal
            {                                                                          //o redenumim
                netermin=getProd(elem2);                                          //facem "rost" de ce era in membrul drept al productiei
                productii[elem.first].erase(elem2);
                for(auto it:netermin)
                    productii[elem.first].insert(it);
            }
            netermin.clear();
    }
    for(auto it:Redenumiri)             //ma uit sa vad,daca mai apare in alte productii nu o sterg
    {   if(toDel(it))
        {productii.erase(it);
        neterminale.erase(it);
        }
    }

    Redenumiri=cautaReden();
    for(auto elem:productii)
    {
        for(auto elem2:elem.second)
        {
            if(elem2.length()>1 )      //daca productia mai are si altceva atunci adaugam la final redenumirile
            {
                for(int i=0;i<elem2.length();i++)
                {
                    string s(1,elem2[i]);
                    if(isReden(s,Redenumiri) && elem.first!=s)
                    {
                      netermin=getProd(s);
                      for(auto it:netermin)
                       productii[elem.first].insert(it);

                    }
                    netermin.clear();
                }

            }
            else if(elem2.length()==1 && isReden(elem2,Redenumiri))productii[elem.first].erase(elem2);
        }
    }

}
string CFG::inlocuieste(string ch,string var,int poz)
{
    for(int i=0;i<ch.length();i++)
        {string s(1,ch[i]);
        if(s==var)
            {
                    ch.replace(i,1,simplificari[s]);
                    i+=simplificari[s].length();
                }
            }


    return ch;
}
void CFG::stepFive()
{
         //retinem o mapa in care cheia este vechiul terminal
                                            //iar valoarea e noua redenumire
    for(auto elem:productii)
    {
        for(auto elem2:elem.second)
        {
            if(elem2.length()>1)   //daca am o lungime mai amre de 1 atunci terminalele le inlocuiesc cu "X"+ un numar;
            {
                for(int i=0;i<elem2.length();i++)
                {   string s(1,elem2[i]);
                    if(terminale.find(s)!=terminale.end())
                    {   contor++;
                        string txt="";
                        txt="X"+to_string(contor);
                        if(simplificari.find(s)==simplificari.end())simplificari.insert({s,txt}); //verfic daca acel neterminal este deja format,daca nu,atunci il formez
                        string ch=elem2;
                        productii[elem.first].erase(ch);  //sterg ce era inainte si adaug noua productie
                        elem2=inlocuieste(elem2,s,i);          //inlocuiesc aparitiile acelui caracter cu noua forma
                        productii[elem.first].insert(elem2);

                    }
                }
            }
        }
    }
}
int CFG::detLungime(string ch)   //metoda in care determin lungimea unei productii ignorand numerele pe care le-am pus pentru X
{int lungime=0;
    for(int i=0;i<ch.length();i++)
    {
        string s(1,ch[i]);
        if(s=="X" || productii.find(s)!=productii.end())lungime++;
    }
    return lungime;
}
void CFG::config()
{
    productii["S"].insert("aBcDeF");
    productii["S"].insert("HF");
    productii["S"].insert("HBc");
    productii["B"].insert("b");
    productii["B"].insert("lambda");
    productii["D"].insert("lambda");
    productii["D"].insert("d");
    productii["F"].insert("G");
    productii["G"].insert("f");
    productii["G"].insert("g");
    productii["H"].insert("lambda");
    terminale.insert("a");
    terminale.insert("b");
    terminale.insert("c");
    terminale.insert("d");
    terminale.insert("e");
    terminale.insert("f");
    terminale.insert("g");
    neterminale.insert("S");
    neterminale.insert("B");
    neterminale.insert("D");
    neterminale.insert("F");
    neterminale.insert("G");
    neterminale.insert("H");
}
void CFG::stepSix()
{
     for(auto elem:productii)
    {
        for(auto elem2:elem.second)
        {int lungime=detLungime(elem2);
            if(lungime>2)                    //daca in membrul drept am productii mai lungi de 2,atunci retin primul terminal/neterminal
                                                    //iar ce este dupa notez cu un Y(nr)
            {
                for(int i=0;i<elem2.length();i++)
                {   string s(1,elem2[i]);
                    int poz=i;
                    string txt="";
                    txt+=s;
                    string nou="";
                    if(s=="X")          //daca primul caracter e X caut sa vad cate cifre are dupa el,
                                        //pentru a afla pozitia de pe care inlocuiesc cu Y
                    {
                        for(int j=i+1;j<elem2.length();j++)
                        {   string ss(1,elem2[j]);poz=j;txt+=ss;
                            if(s=="X" || productii.find(ss)!=productii.end() || terminale.find(ss)!=terminale.end())break;

                        }
                    }
                    for(int j=poz+1;j<elem2.length();j++)
                        {
                            string aux(1,elem2[j]);
                            nou+=aux;
                        }
                    productii[elem.first].erase(elem2);
                    if(pas6.find(nou)==pas6.end())
                    {
                        contor++;
                        txt=txt+"Y"+to_string(contor);
                        pas6.insert({nou,"Y"+to_string(contor)});
                        productii[elem.first].insert(txt);
                    }
                    else{
                        txt=txt+pas6[nou];
                        productii[elem.first].insert(txt);
                    }


                    productii[elem.first].insert(txt);
                    break;
                }
            }
        }
    }

}
istream& operator>>(istream& in,CFG& M)
{   M.productii.clear();                        //ma asigur ca nu am nici o productie pusa
                                                //si ca nu mi se va afisa acel spatiu pus
                                                //pentru cosntructorul fara parametri in cazul
                                                //in care se apeleaza
    int numarProd,nrTerminale;
    cout<<"Introduceti numarul de productii:";
    in>>numarProd;
    cout<<"Introduceti numarul terminalelor:";
    in>>nrTerminale;
    string ch,produc;
    for(int i=0;i<nrTerminale;i++)
    {   cout<<"Introduceti terminalele:";
        in>>ch;
        M.terminale.insert(ch);
    }
    cout<<"Introduceti productile (membru stang mebru drept)";
    for(int j=0;j<numarProd;j++)
    {
        in>>produc;                         //citesc productia si o adaug,daca o adaugam de fiecare data cand o citim
                                            //nu influenteaza cu nimic programul,deoarece adaugam multimea de productii
                                            //este un set,deci nu avem duplicate
        M.neterminale.insert(produc);
        in>>ch;                             //adaugam perechea mebru stang->membru drept
        M.productii[produc].insert(ch);
    }
    return in;
}
ostream& operator<<(ostream& out, CFG& M)
{
    for(auto elem: M.getProductii())
    {   cout<<elem.first<<"->";
        for(auto elem2:elem.second)
            cout<<elem2<<" | ";
        cout<<endl;

    }
    cout<<"Unde X(numar) reprezinta:"<<endl;
    for(auto el:M.getSimplificari())
    {
        cout<<el.first<<" :"<<el.second<<" ";
    }
    cout<<endl;
    cout<<"Unde Y(numar) reprezinta:"<<endl;
    for(auto ele:M.getPas())
    {
        cout<<ele.first<<" : "<<ele.second<<endl;
    }
    return out;
}

int main()
{
    CFG m;
    m.config();
    m.stepOne();
    m.stepTwo();
    m.stepThree();
    m.stepFive();
    m.stepSix();
    cout<<m;
    return 0;
}
