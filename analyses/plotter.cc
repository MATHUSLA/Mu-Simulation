void plotter()
{
    TFile* f = TFile::Open("data/20180719/201546/run0.root");
    TH3F* rat = new TH3F("rat", "", 50, 0, 20000, 50, -5000, 5000, 50, -500, 0);
    


}
