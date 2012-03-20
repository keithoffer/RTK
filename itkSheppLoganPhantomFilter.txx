#ifndef __itkSheppLoganPhantomFilter_txx
#define __itkSheppLoganPhantomFilter_txx

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include "rtkHomogeneousMatrix.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
void SheppLoganPhantomFilter<TInputImage, TOutputImage>::Config()
{
const char *search_fig="Ellipsoid", *search_num="NUMBER"; // Set search pattern
int offset=0, m=0;
std::string line;
std::ifstream Myfile;
bool number_flag=false;
std::stringstream ss(std::stringstream::in | std::stringstream::out);
std::string temp, temp_ellip;
std::string parameters[8]={"x","y","z","A","B","C","beta","gray"};

Myfile.open(m_ConfigFile.c_str());
if(Myfile.is_open())
{
  while(!Myfile.eof())
  {
    getline(Myfile,line);
    if ((offset = line.find(search_fig, 0)) != std::string::npos) //Ellipsoid found
    {
      m++;
      for (int j=0; j < 8; j++)
      {
        temp_ellip = "\0";
      if ((offset = line.find(parameters[j], 0)) != std::string::npos)
      {
        for (int k=offset; k < offset+10; k++)
        {
        if (isdigit(line[k]) || line[k]=='-')
        {
          for (int b=k; b<offset+10; b++)
          {
            temp_ellip += line[b];
          }
        break;
        }
        }
      double param_temp=atof(temp_ellip.c_str());
      //Saving all parameters for each ellipsoid
      m_Fig[m-1][j]=param_temp;
      }
      }
    }
    if (number_flag==true) //Getting number of figures
    {
      number_flag=false;
      for (unsigned int i=0; i < line.size(); i++)
      {
        if (isdigit(line[i]))
        {
          for (unsigned int a=i; a<line.size(); a++)
          {
            temp += line[a];
          }
        break;
        }
      }
    int number_int=0;
    ss << temp;
    ss >> number_int;
    m_NumberArg=number_int;
    }
    if ((offset = line.find(search_num, 0)) != std::string::npos) //Number found
    {
      number_flag=true;
    }
  }
Myfile.close();
}
else
  std::cout<<"Error opening File\n"<<std::endl;
}

template <class TInputImage, class TOutputImage>
void SheppLoganPhantomFilter<TInputImage, TOutputImage>::GenerateData()
{
std::cout << "Configuration Process...\n"<< std::endl;
this->Config();

REIType::Pointer rei[m_NumberArg];
for (int i=0; i<m_NumberArg; i++)
{
  rei[i] = REIType::New();
  rei[i]->m_i=i+1;

  rei[i]->SetMultiplicativeConstant(m_Fig[i][7]); //Set GrayScale value
  rei[i]->SetSemiPrincipalAxisX(m_Fig[i][0]);
  rei[i]->SetSemiPrincipalAxisY(m_Fig[i][1]);
  rei[i]->SetSemiPrincipalAxisZ(m_Fig[i][2]);


  rei[i]->SetCenterX(m_Fig[i][3]);
  rei[i]->SetCenterY(m_Fig[i][4]);
  rei[i]->SetCenterZ(m_Fig[i][5]);

  if(m_Fig[i][6]<(-0.1))
  {
    rei[i]->SetRotate(true);
    rei[i]->SetRotationAngle(m_Fig[i][6]);
  }

  if (i==(m_NumberArg-1)) //last case
  {

    if(i==0) //just one ellipsoid
      rei[i]->SetInput( rei[i]->GetOutput() );
    else
    {
      rei[i]->SetInput( rei[i-1]->GetOutput() );
    }
    rei[i]->SetGeometry( this->GetGeometry() );
  }

  if (i>0) //other cases
  {
    rei[i]->SetInput( rei[i-1]->GetOutput() );
    rei[i]->SetGeometry( this->GetGeometry() );
  }

  else //first case
  {
    rei[i]->SetInput( this->GetInput() );
    rei[i]->SetGeometry( this->GetGeometry() );
  }
}
//Update
rei[(m_NumberArg)-1]->Update();
this->GraftOutput( rei[(m_NumberArg)-1]->GetOutput() );
}
}// end namespace itk

#endif
