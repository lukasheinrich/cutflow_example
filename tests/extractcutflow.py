import ROOT
import yaml
import sys
cutnames = ['none','dimuon','lead>40','sublead>30']
f = ROOT.TFile.Open(sys.argv[1])
h = f.Get('Cutflow')
nbins = h.GetNbinsX()
events = [h.GetBinContent(i+1) for i in range(nbins)]
with open(sys.argv[2],'w') as f:
    f.write(yaml.dump([{'name':n,'value':e} for n,e in zip(cutnames,events)], default_flow_style = False))
