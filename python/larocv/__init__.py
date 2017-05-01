import ROOT
ROOT.gSystem.Load("libLArOpenCV_ImageClusterAlgoModule.so")
from ROOT import larocv
larocv.AlgoFunctionLoader
