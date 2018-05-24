#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <math.h>
#include <list> 
#include <algorithm>
#include <unordered_map>
#include <sstream>

using namespace std;


string filename;
string cachesize;
string blocksize;
string ways;



long Sets;
int OB, OBMsk, IB, IBMsk, TB, TBMsk;


int mask(int s){
    return (1 << s) -1;
}

int getvalue(int address, int mask, int shift){
      return (address & mask ) >> shift;
}

int main(int argc, char* argv[])
{
    double CZ = 512*1024;
double BZ = 32;
double WS = 4;
    
    unordered_map<long , list<long> > cache;
    
    long miss = 0;
    long allrequest = 0;
    long hit = 0;
    
    if( argc == 2)
    {
        //cout<<argc<<endl;
        filename = argv[1];
        cout<<filename<<endl;

    }
    
    else if( argc == 5)
    {
        filename = argv[1];
        cachesize = argv[2];
        blocksize = argv[3];
        ways = argv[4];
        
        if(cachesize.find("K") != -1 )
        {
            cachesize =cachesize.substr(0, cachesize.length()-1);
            CZ =atof(cachesize.c_str());
            CZ = CZ * 1024;
        }
        else if (cachesize.find("M") != -1)
        {
            cachesize =cachesize.substr(0, cachesize.length()-1);
            CZ =atof(cachesize.c_str());
            CZ = CZ * 1024*1024;
        }
        else
        {
            CZ =atof(cachesize.c_str());
        }
        BZ = atof(blocksize.c_str());
        WS = atof(ways.c_str());
        /*
        Sets = CZ/(BZ*WS);
        OB = log10(BZ)/log10(2);
        OBMsk = mask(OB);
        
        IB =log10(Sets)/log10(2);
        IBMsk = mask(IB)<<OB;
        
        TB = 32 - OB - IB;
        TBMsk = mask(TB)<<(OB+IB);
        */
        
        
    }
    else
    {
        cout<<"Expecting arguments:{tracefilename} {cache size} {blocksize} {ways of associativity} or "<<endl;
        cout<<"{tracefilename} "<<endl;
    }
    
    if (WS == 0)
    {
        Sets = CZ/BZ;
    }
    else
    {
        Sets = CZ/(BZ*WS);
    }
    OB = log10(BZ)/log10(2);
    OBMsk = mask(OB);
    
    if ( WS == 0)
    {
        IB = 0;
        IBMsk = 0;
    }
    else
    {
        IB =log10(Sets)/log10(2);
        IBMsk = mask(IB)<<OB;
    }
    
    
    TB = 32 - OB - IB;
    TBMsk = mask(TB)<<(OB+IB);
    
    cout<<"filename:"<<filename<<endl;
    cout<<"cachesize:"<<CZ<<endl;
    cout<<"blocksize:"<<BZ<<endl;
    cout<<"ways:"<<WS<<endl;
    cout<<"# of Sets:"<<Sets<<endl;
    cout<<"offset Bits:"<<OB<<endl;
    cout<<"Index Bits:"<<IB<<endl;
    cout<<"Tag Bits:"<<TB<<endl;
        
    
    //initialize cache
    if (WS == 0)
    {
        list<long> *List =new list<long>;
        cache.insert(make_pair(0,*List));
    }
    else
    {
        for (long i = 0; i<Sets;i++)
        {
            list<long> *List =new list<long>;
            cache.insert(make_pair(i,*List));
        }
        
    }
    //cout<<"size!!!!"<<cache.size()<<endl;
    
    /*
     doc.open(filename.c_str());
        if (doc.fail())
        {
            cout<<"Document is not exsit!"<<endl;
            doc.close();
        }
        else
        {
        */
    string line;
    ifstream infile(filename);
    while (getline(infile, line)){
        
        istringstream iss(line);
         string instr, aoffset, address;
        long decoffset, decadress, realadress;
        
        if (!(iss >> instr >> aoffset >> address)) { break; }
        allrequest += 1;
        decoffset = stol (aoffset,nullptr,10);
        decadress = stoul(address, nullptr, 16);
        realadress = decadress + decoffset;
        
        long OFFSET, INDEX, TAG;
        OFFSET = getvalue(realadress,OBMsk,0);
        INDEX = getvalue(realadress,IBMsk,OB);
        TAG = getvalue(realadress,TBMsk,OB+IB);
        
        list<long> T;
        if (WS == 0)
        {
            T = cache[0];
            auto itra = find(T.begin(), T.end(), TAG);
     
            if(itra == T.end())
            {
                //cout<<"no element"<<endl;
                miss += 1;
                //cout<<miss<<endl;
                if (T.size()<Sets)
                {
                        T.push_front(TAG);
                        //cout<<"we pushed sthing"<<endl;
                        
                  }
                  else
                  {
                        T.pop_back();
                        T.push_front(TAG);
                        //cout<<"we delete and pushed sth"<<endl;
                  }
            }
            else
            {
                hit += 1;
                //cout<<hit<<endl;
                  T.erase(itra);
                  T.push_front(TAG);
                 // cout<<"change position"<<endl;
            }
            
            cache[0] = T;
            
        }
        else
        {
            unordered_map<long, list<long> >::iterator it;
            it = cache.find(INDEX);
            T = it->second;
     
            auto itra = find(T.begin(), T.end(), TAG);
     
            if(itra == T.end())
            {
                //cout<<"no element"<<endl;
                miss += 1;
                if (T.size()<WS)
                {
                        T.push_front(TAG);
                        //cout<<"we pushed sthing"<<endl;
                        
                  }
                  else
                  {
                        T.pop_back();
                        T.push_front(TAG);
                        //cout<<"we delete and pushed sth"<<endl;
                  }
            }
            else
            {
                hit += 1;
                  T.erase(itra);
                  T.push_front(TAG);
                 // cout<<"change position"<<endl;
            }
            
            cache[INDEX] = T;
        }
        
        
        
    }
            
         //}
        
        cout<<"Miss time is:"<<miss<<endl;
        cout<<"Hit time is:"<<hit<<endl;
        float Missrate = float(miss)/float(allrequest)*100;
        cout<<"Missrate is :"<<Missrate<<"%"<<endl;
        cout<<"Hitrate is:"<<100-Missrate<<"%"<<endl;
    
    return 0;
}
