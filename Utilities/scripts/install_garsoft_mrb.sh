#!/bin/sh

#  install_garsoft_mrb.sh
#  Install the ups products needed by novasoft
#  Created by Brian Rebel on 9/15/16.

if [ "$#" -ne 5 ]; then
  echo "Usage: install_garsoft_mrb.sh <external ups directory> <garsoft base version> <garsoft qualifiers> <buildtype> <garsoft destination directory>"
  echo " For example: install_garsoft_mrb.sh $HOME/gar/products vXX_YY_ZZ sJJ:eII <debug | prof> $HOME/gar/"
 #echo " the lastest version of the garsoft bundle can be found at http://scisoft.fnal.gov/scisoft/bundles/garsoft/"
  echo " use the lastest version of the larsoft bundle to get external products, the bundle can be found at http://scisoft.fnal.gov/scisoft/bundles/larsoft/"
  echo " If running on Fermilab dunegpvm nodes, set the <external ups directory> to be /cvmfs/fermilab.opensciencegrid.org/products/larsoft"
  exit
fi

PRODDIR=${1}
BASEV=${2}
QUALS=${3}
BUILDTYPE=${4}
GARDIR=${5}

BUNDLEQUALS=`echo ${QUALS} | sed -e 's/:/-/g'`

echo "Check inputs: "
echo "   External UPS destination: ${PRODUCTS}"
echo "   GArSoft Base version:     ${BASEV}"
echo "   GArSoft Qualifiers:       ${QUALS}"
echo "   Build type:               ${BUILDTYPE}"
echo "   GArSoft destination:      ${GARDIR}"

# determine the OS
OS=`uname`
if [ "${OS}" = "Linux" ]
then
  flvr=slf`lsb_release -r | sed -e 's/[[:space:]]//g' | cut -f2 -d":" | cut -f1 -d"."`
elif [ "${OS}" = "Darwin" ]
then
  flvr=d`uname -r | cut -f1 -d"."`
  QUALS="${QUALS}:osx"
else
  echo "ERROR: unrecognized operating system ${OS}"
  exit 1
fi

# check the product directory to see if we are wanting to use the gpvm node
# product installation or not

if [[ ${PRODDIR} != *"fermilab"* ]] ; then
  cd ${PRODDIR}
  curl -O http://scisoft.fnal.gov/scisoft/bundles/tools/pullProducts
  chmod +x pullProducts
  ./pullProducts ${PRODDIR} ${flvr} garsoft-${BASEV} ${BUNDLEQUALS} ${BUILDTYPE}

  echo "NB the bundle may not yet be available. If that is the case, try developing on the dunegpvm nodes at FNAL"
fi

# source the ups products setup
source ${PRODDIR}/setup

# setup mrb and create the project
echo "setting up the mrb working area"

if [ ! -d "${GARDIR}" ]; then
  echo "GArSoft destination directory not found, make it"
  mkdir ${GARDIR}
fi

cd ${GARDIR}

setup mrb

export MRB_PROJECT=garsoft

mrb newDev -v develop -q ${QUALS}:${BUILDTYPE}

# make the local products directory name a bit more generic
BUNDLEQUALS=`echo ${QUALS} | sed -e 's/:/_/g'`

mv localProducts_${MRB_PROJECT}_develop_${BUNDLEQUALS}_${BUILDTYPE} localProducts_${MRB_PROJECT}

sed -i~ 's/localProducts_'"${MRB_PROJECT}"'_develop_'"${BUNDLEQUALS}_${BUILDTYPE}"'/localProducts_'"${MRB_PROJECT}"'/g' localProducts_${MRB_PROJECT}/setup

rm localProducts_${MRB_PROJECT}/*~

source localProducts_${MRB_PROJECT}/setup

echo "checking out the garsoft code"

cd ${MRB_SOURCE}
mrb g -d garsoft garsoft-garsoft
mrb uc

echo "set the build environment"
cd ${MRB_BUILDDIR}
mrbsetenv

echo "If no errors were reported, you are good to go"
echo " "
echo "Here is an example function you can add to your $HOME/.bash_profile to setup for garsoft development:"
echo " "
echo "develop_gar ()"
echo "{"
echo " "
echo " source ${PRODDIR}/setup"
echo " setup mrb"
echo " setup ninja v1_6_0b #optional if using ninja to build the code, requires mrb i --generator ninja"
echo " source ${GARDIR}/localProducts_${MRB_PROJECT}/setup"
echo " cd ${MRB_BUILDDIR}"
echo " mrbsetenv"
echo " echo ready to build in ${PWD} "
echo "}"

echo " "
echo "Here is an example function you can add to your $HOME/.bash_profile to setup garsoft for running:"
echo " "
echo "setup_gar ()"
echo "{"
echo " "
echo " source ${PRODDIR}/setup"
echo " export PRODUCTS=/grid/fermiapp/products/common/db:${PRODUCTS}"
echo " export PRODUCTS=${GARDIR}/localProducts_${MRB_PROJECT}/:${PRODUCTS}"
echo " "
echo "  # setup sam to be able to find files"
echo "  #kx509"
echo "  #kxlist -p"
echo "  #voms-proxy-init --rfc --voms=fermilab:/fermilab/gar/Role=Analysis --noregen"
echo "  #export X509_USER_PROXY=/tmp/x509up_u`id -u`"
echo " "
echo " #setup the garsoft ups product"
echo " setup garsoft develop -q ${QUALS}:${BUILDTYPE}"
echo " cd ${GARDIR}"
echo " "
echo " echo ready to run gar in ${GARDIR} "
echo "}"

