#ifndef ANALYSIS_UTILITY_FUNCTIONS.H
#define ANALYSIS_UTILITY_FUNCTIONS.H

#include <iostream>
#include <fstream>
#include <sstream>
#include "TString.h"
#include "TMath.h"
#include <vector>


//This belongs to the header file, not the class. So that way an output file can come from multiple data objects

 string out_dir_temp = "yields_output";
 TString output_directory = TString(out_dir_temp);
 string output_temp = "";
 TString output_file = TString(output_temp);


//Define some fits to be used for analysis

//Background fit of a second-order polynomial
double BG_fit(double *x, double *param){
double c = param[0];
double b = param[1];
double a = param[2];

double func = a*(pow(x[0],2))+b*(x[0])+c;
return func;
}


//Fit for protons using a Gaussian
double P_fit(double *x, double *param){
double amp = param[0];
double offset = param[1];
double sigma = param[2];

double func = amp*(exp(-0.5*pow((x[0]-offset)/sigma,2)));
return func;
}

//Fit for neutrons using a Gaussian
double N_fit(double *x, double *param){
double amp = param[0];
double offset = param[1];
double sigma = param[2];

double func = amp*(exp(-0.5*pow((x[0]-offset)/sigma,2)));
return func;
}

//Total fit to overlay. Combine all fits
double Tot_fit(double *x, double *param){
double tot_func = BG_fit(x,&param[0])+P_fit(x,&param[3])+N_fit(x,&param[6]);
return tot_func;

}

//Function to intialize fit parameters. Currently only supports SBS-4. Will need to have support other kinematics like SBS8,SBS9
vector<Double_t> fit_Params(TString myKin){
vector<Double_t> param (11);
//need to look at the on a graph and see what its actually trying to do
if(myKin == "SBS4"){
param[0] = 200; //used for total fit as 0 parameter
param[1] = -0.3; //used for total fit as 1 parameter
param[2] = 0.8; //used for total fit as 2 parameter
param[3] = 9400; //used for total fit as 3 parameter
param[4] = -0.35; //used for total fit as 4 parameter
param[5] = 0.2; //used for total fit as 5 parameter
param[6] = 3300; //used for total fit as 6 parameter
param[7] = 0.28; //used for total fit as 7 parameter
param[8] = 0.19; //used for total fit as  8 parameter
param[9] = -1.5; // min value for fit
param[10] = 0.7; // max value for fit
}else{
//Error message
cout << "Error: The kinematic setting you are analyzing does not have preset fit parameters. Plots will probably not make sense!";
}

return param;
}



//convert an int to a TString. Just a helper function
TString intToTString(int datInt){
stringstream ss;
ss << datInt;
std::string datInt_temp;
ss >> datInt_temp;
TString datInt_string(datInt_temp.c_str());
//cout << datInt_string << endl;
return datInt_string;
}
//convert from degrees to radians
double DegToRad(double myNum){
return (myNum * (TMath::Pi()/180.0));
}
double RadToDeg(double datRad){
return ((datRad * 180.0)/(TMath::Pi()));
}

TString getOutputDir(){
return output_directory;
}
TString makeOutputFileName(TString exp, TString Kin, int SBS_field,TString target){
TString outfile = Form("%s/yields_Zeke_%s_%s_%s_%i.root",(getOutputDir()).Data(),exp.Data(),Kin.Data(),target.Data(),SBS_field);
//cout << outfile << endl;
return outfile;
}

//Might be rewriting some code here. But make a class which can then be used to parse the kinematic file info
class kinematic_obj{
//Define some private variables we will fill later.
private:
TString kinematic;
double Ebeam,bbtheta,bbdist,sbstheta,sbsdist,hcaltheta,hcaldist,Q2;
public:
//Let's make a constructor which will just directly parse the kinematic file and store the info 
kinematic_obj(const char *kinematic_file_name,TString Kin){
	kinematic = Kin;
	//Now read-in the kinematic information
	ifstream kinfile(kinematic_file_name);
	//check if there is a problem opening the file
	if(kinfile.fail()){
	cout << "There was a problem with the kinematic file " << kinematic_file_name << ". Figure it out nerd!" << endl;
	return;
	}
	TString datLine;
	TString datKin;
	bool gotKin = false;
	while(datLine.ReadLine(kinfile)){
		if(datLine.BeginsWith(kinematic)){
		//We found the right kinematic. Store the info
		TObjArray *myobjs = datLine.Tokenize(" ");
		//Assuming ordering is kinematic, Beam Energy, BB Angle, SBS angle, SBS dist, HCal dist, Expected Q^2
 		datKin = ((TObjString*) (*myobjs)[0])->GetString();
                Ebeam = (((TObjString*) (*myobjs)[1])->GetString()).Atof();
                bbtheta = (((TObjString*) (*myobjs)[2])->GetString()).Atof();
                bbdist = (((TObjString*) (*myobjs)[3])->GetString()).Atof();
                sbstheta = (((TObjString*) (*myobjs)[4])->GetString()).Atof();
                sbsdist = (((TObjString*) (*myobjs)[5])->GetString()).Atof();
                hcaltheta = (((TObjString*) (*myobjs)[6])->GetString()).Atof();
                hcaldist = (((TObjString*) (*myobjs)[7])->GetString()).Atof();
		Q2 = (((TObjString*) (*myobjs)[8])->GetString()).Atof();
                //cout << "Kinematic " << datKin << "  Beam Energy " << Ebeam << " BB Angle  " << bbtheta << " BB Dist " << bbdist << " SBS Angle  " << sbstheta << " SBS Dist  " << sbsdist << " HCal Angle " << hcaltheta <<  " HCal Dist  " << hcaldist << " Q^2 " << Q2  << endl;
		gotKin = true;
                //cout << gotRun << endl;
 		}
                else{
                //Where are still searching or its a comment
                //cout << "Cond 3" << endl; 
                 continue;
                }
        }
        if ((kinfile.eof()) && !gotKin){
         //Conditional that we checked the entire kinematic file and did not find the kinematic info
	 cout << "Did not find kinematic: " << datKin << " in the kinematic file! Quitting, figure it out!" << endl;
        return;
        }
 }
 TString getKinematic(){
        return kinematic;
 }
 double getBeamEnergy(){
        return Ebeam;
        }
 double getBBAngle_Deg(){
        return bbtheta;
        }
 double getBBAngle_Rad(){
        return DegToRad(bbtheta);
        }
 double getBBDist(){
        return bbdist;
        }
 double getSBSAngle_Deg(){
        return sbstheta;
        }
 double getSBSAngle_Rad(){
        return DegToRad(sbstheta);
        }
 double getSBSDist(){
        return sbsdist;
        }
 double getHCalAngle_Deg(){
        return hcaltheta;
        }
 double getHCalAngle_Rad(){
        return DegToRad(hcaltheta);
        }
 double getHCalDist(){
        return hcaldist;
        }
 double getQ2(){
        return Q2;
        }
 
 void printKinInfo(){
        cout << "------------------------"                              << endl
             << Form("Kinematic: %s,",(getKinematic()).Data())          << endl
             << Form("Beam Energy: %f,",getBeamEnergy())                << endl
             << Form("BB angle in Degrees: %f,",getBBAngle_Deg())       << endl
             << Form("BB angle in Radians: %f,",getBBAngle_Rad())       << endl
             << Form("BB Distance: %f,",getBBDist())                    << endl
             << Form("SBS angle in Degrees: %f,",getSBSAngle_Deg())     << endl
             << Form("SBS angle in Radians: %f,",getSBSAngle_Rad())     << endl
             << Form("SBS Distance: %f,",getSBSDist())                  << endl
             << Form("HCal angle in Degress: %f,",getHCalAngle_Deg())   << endl
             << Form("HCal angle in Radians: %f,",getHCalAngle_Rad())   << endl
             << Form("HCal Distance: %f,",getHCalDist())                << endl
             << "------------------------"                              << endl;
 }
};



//Make a class to store information about the data
class data_object{
//Define some private variables we will fill later.
private:
TString pass,kinematic,target,input_file;
double Ebeam,bbtheta,bbdist,sbstheta,sbsdist,hcaltheta,hcaldist;
int run,sbs_field;
string input_directory = "/work/halla/sbs/sbs-gmn";

TString makeInputFileName(){
 //All of this was to have a modular input directory. So let's make it
 const char *input_directory_char = input_directory.c_str();
 const char *pass_char = pass.Data();
 const char *kin_char = kinematic.Data();
 const char *tar_char = target.Data();
 TString inputfile = Form("%s/%s/%s/%s/rootfiles/e1209019_fullreplay_%i_*.root",input_directory_char,pass_char,kin_char,tar_char,run);
 //cout << "File Location " << inputfile << endl;               
 return inputfile;
}

//constructor to search through the files we care about and store the information of use. Will handle some cases of unexpected behavior. Handle on a run by run basis. But one could store these in a vector and manipulate them
public:
 data_object(int runnum,const char *data_file_name,const char *kinematic_file_name,TString Kin, TString SBS_field, TString targ){
 
 //This part of the constructor reads in information about the data file itself
 ifstream datafile(data_file_name);
 //check if there is a problem opening the file
 if(datafile.fail()){
 cout << "There was a problem with the data file " << data_file_name << ". Figure it out nerd!" << endl;
 return;
 }
 TString currentLine;
 bool gotRun = false;
 TString runnum_string = intToTString(runnum);
 while(currentLine.ReadLine(datafile)){
	if(currentLine.BeginsWith(runnum_string)){
	 //We found the right run number. Store the info
	 TObjArray *tokens = currentLine.Tokenize(" ");
	 //Assuming ordering is runnum, pass, SBSKin, Target, sbsfield
	 run =  (((TObjString*) (*tokens)[0])->GetString()).Atoi();
	 pass = ((TObjString*) (*tokens)[1])->GetString();
	 kinematic =  ((TObjString*) (*tokens)[2])->GetString();
	 target =  ((TObjString*) (*tokens)[3])->GetString();
	 sbs_field = (((TObjString*) (*tokens)[4])->GetString()).Atoi();
	 //cout << "Run " << run  << " Pass " << pass << " Kin " << kinematic << " Target " << target << " SBS Field  " << sbs_field << endl;
	if(!(kinematic == Kin)){
	cout << "The run " << run << " has a mismatch in the kinematic, investigate what is going on!" << endl;
	return;
	}
	if(!(sbs_field == SBS_field)){
        cout << "The run "<< run << " has a mismatch in the sbs field, investigate what is going on!" << endl;
        return;
        }
 	if(!(target == targ )){
        cout << "The run " << run << " has a mismatch in the target, investigate what is going on!" << endl;
        return;
        }
	gotRun = true;
	 //cout << gotRun << endl;
	 }
	 else{
	 //Where are still searching but it's a comment denoted by #
	 // cout << "Cond 2" << endl; 
	 continue;
	 }
 }
 if ((datafile.eof()) && !gotRun){
 //Conditional that we checked the entire data file and did not find the runnum
 cout << "Did not find run number: " << runnum << " in the data file! Quitting, figure it out!" << endl;
 return;
 }  
 //Now read-in the kinematic information and store the info
 kinematic_obj datKin(kinematic_file_name, Kin);
 
 Ebeam = datKin.getBeamEnergy();
 bbtheta = datKin.getBBAngle_Deg();
 bbdist = datKin.getBBDist();
 sbstheta = datKin.getSBSAngle_Deg();
 sbsdist = datKin.getSBSDist();
 hcaltheta = datKin.getHCalAngle_Deg();
 hcaldist = datKin.getHCalDist();

 input_file = makeInputFileName();
}

 int getRun(){
	return run;
	}
 TString getPass(){
	return pass;
	}
 TString getKinematic(){
	return kinematic;
	}
 TString getTarget(){
	return target;
	}
 int getSBSField(){
	return sbs_field;
	}

 double getBeamEnergy(){
	return Ebeam;
	}		
 double getBBAngle_Deg(){
	return bbtheta;
	}
 double getBBAngle_Rad(){
	return DegToRad(bbtheta);
	}
 double getBBDist(){
	return bbdist;
	}
 double getSBSAngle_Deg(){
        return sbstheta;
        }
 double getSBSAngle_Rad(){
        return DegToRad(sbstheta);
        }
 double getSBSDist(){
        return sbsdist;
        }
 double getHCalAngle_Deg(){
        return hcaltheta;
        }
 double getHCalAngle_Rad(){
        return DegToRad(hcaltheta);
        }
 double getHCalDist(){
        return hcaldist;
        }
 string getInputDir(){
	return input_directory;
	}
 TString getInputFile(){

	return input_file;
	}
 void printRunInfo(){
        cout << "------------------------"                              << endl
             << Form("Run number: %i,",getRun())                        << endl
             << Form("Kinematic: %s,",(getKinematic()).Data())          << endl
             << Form("Target: %s,", (getTarget()).Data())               << endl
             << Form("SBS Field: %i,",getSBSField())       		<< endl
             << Form("Beam Energy: %f,",getBeamEnergy())                << endl
             << Form("BB angle in Degrees: %f,",getBBAngle_Deg())       << endl
             << Form("BB angle in Radians: %f,",getBBAngle_Rad())       << endl
             << Form("BB Distance: %f,",getBBDist())                    << endl
             << Form("SBS angle in Degrees: %f,",getSBSAngle_Deg())     << endl
             << Form("SBS angle in Radians: %f,",getSBSAngle_Rad())     << endl
             << Form("SBS Distance: %f,",getSBSDist())                  << endl
             << Form("HCal angle in Degress: %f,",getHCalAngle_Deg())   << endl
             << Form("HCal angle in Radians: %f,",getHCalAngle_Rad())   << endl
             << Form("HCal Distance: %f,",getHCalDist())                << endl
             << "------------------------"                              << endl;
 }
};
#endif
