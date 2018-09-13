
#include <functional>

#include "../../../helper.hh"

#define NEUTRON_PDG 2112

//__Get Or Make Object from ROOT File___________________________________________________________
template<class Object, class ...Args>
Object* get_or_make_object(TFile* file,
                           const std::string& key,
                           Args&& ...args) {
  auto obj = dynamic_cast<Object*>(file->Get(key.c_str()));
  return obj ? obj : new Object(args...);
}
//----------------------------------------------------------------------------------------------

//__Apply Clear Function to All Objects_________________________________________________________
template<class Arg>
void clear_all(Arg&& arg) {
  arg.clear();
}
template<class Arg,
         class ...Args>
void clear_all(Arg&& arg,
               Args&& ...args) {
  clear_all(arg);
  clear_all(args...);
}
//----------------------------------------------------------------------------------------------

namespace MATHUSLA { namespace MU { ////////////////////////////////////////////////////////////

//__Analyis of Neutrons in Cosmic Rays__________________________________________________________
bool is_neutron_upward(const double_vector& t,
                       const double_vector& px,
                       const double_vector& py,
                       const double_vector& pz) {
  const auto size = t.size();
  if (size == 0UL || size != px.size() || size != py.size() || size != pz.size())
    return false;

  std::vector<std::size_t> indices;
  indices.reserve(size);
  for (std::size_t i{}; i < size; ++i)
    indices.push_back(i);

  std::sort(indices.begin(), indices.end(),
    [&](const std::size_t left, const std::size_t right) { return t[left] < t[right]; });

  // preliminary
  for (const auto& i : indices) {
    if (pz[i] < 0.0L)
      return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------

//__Analyis of Neutrons in Cosmic Rays__________________________________________________________
void neutron_analysis(const char* input,
                      const char* output,
                      bool update=true) {
  TFile out(output, update ? "UPDATE" : "RECREATE");
  out.cd();

  std::size_t neutron_count{}, event_count{}, upwards_neutron_count{};
  double_vector* track_id = nullptr;
  double_vector* parent_id = nullptr;
  double_vector* deposit = nullptr;
  double_vector* t = nullptr;
  double_vector* x = nullptr;
  double_vector* y = nullptr;
  double_vector* z = nullptr;
  double_vector* e = nullptr;
  double_vector* px = nullptr;
  double_vector* py = nullptr;
  double_vector* pz = nullptr;

  auto neutron_tree = dynamic_cast<TTree*>(out.Get("neutron"));
  if (!neutron_tree) {
    neutron_tree = new TTree("neutron", "Neutron Tree");
    neutron_tree->Branch("Track", &track_id);
    neutron_tree->Branch("Parent", &parent_id);
    neutron_tree->Branch("Deposit", &deposit);
    neutron_tree->Branch("Time",  &t);
    neutron_tree->Branch("X",  &x);
    neutron_tree->Branch("Y",  &y);
    neutron_tree->Branch("Z",  &z);
    neutron_tree->Branch("E",  &e);
    neutron_tree->Branch("PX", &px);
    neutron_tree->Branch("PY", &py);
    neutron_tree->Branch("PZ", &pz);
  } else {
    neutron_tree->SetBranchAddress("Track", &track_id);
    neutron_tree->SetBranchAddress("Parent", &parent_id);
    neutron_tree->SetBranchAddress("Deposit", &deposit);
    neutron_tree->SetBranchAddress("Time",  &t);
    neutron_tree->SetBranchAddress("X",  &x);
    neutron_tree->SetBranchAddress("Y",  &y);
    neutron_tree->SetBranchAddress("Z",  &z);
    neutron_tree->SetBranchAddress("E",  &e);
    neutron_tree->SetBranchAddress("PX", &px);
    neutron_tree->SetBranchAddress("PY", &py);
    neutron_tree->SetBranchAddress("PZ", &pz);
  }

  double_vector* source_pdg = nullptr;
  double_vector* source_track_id = nullptr;
  double_vector* source_parent_id = nullptr;
  double_vector* source_deposit = nullptr;
  double_vector* source_t = nullptr;
  double_vector* source_x = nullptr;
  double_vector* source_y = nullptr;
  double_vector* source_z = nullptr;
  double_vector* source_e = nullptr;
  double_vector* source_px = nullptr;
  double_vector* source_py = nullptr;
  double_vector* source_pz = nullptr;
  for (const auto& path : helper::search_directory(input, "root")) {
    TFile box_file(path.c_str(), "READ");
    box_file.cd();
    auto box_run = dynamic_cast<TTree*>(box_file.Get("box_run"));
    if (!box_run) {
      box_file.Close();
      continue;
    }

    box_run->SetBranchAddress("PDG", &source_pdg);
    box_run->SetBranchAddress("Track", &source_track_id);
    box_run->SetBranchAddress("Parent", &source_parent_id);
    box_run->SetBranchAddress("Deposit", &source_deposit);
    box_run->SetBranchAddress("Time",  &source_t);
    box_run->SetBranchAddress("X",  &source_x);
    box_run->SetBranchAddress("Y",  &source_y);
    box_run->SetBranchAddress("Z",  &source_z);
    box_run->SetBranchAddress("E",  &source_e);
    box_run->SetBranchAddress("PX", &source_px);
    box_run->SetBranchAddress("PY", &source_py);
    box_run->SetBranchAddress("PZ", &source_pz);

    helper::traverse(box_run,
      [&](const int entries) {
        event_count += entries;
        return entries > 0;
      },
      [&]() {
        clear_all(*track_id, *parent_id, *deposit, *t, *x, *y, *z, *e, *px, *py, *pz);
        const auto size = source_track_id->size();
        for (std::size_t i{}; i < size; ++i) {
          if ((*source_pdg)[i] == NEUTRON_PDG) {
            ++neutron_count;
            track_id->push_back((*source_track_id)[i]);
            parent_id->push_back((*source_parent_id)[i]);
            deposit->push_back((*source_deposit)[i]);
            t->push_back((*source_t)[i]);
            x->push_back((*source_x)[i]);
            y->push_back((*source_y)[i]);
            z->push_back((*source_z)[i]);
            e->push_back((*source_e)[i]);
            px->push_back((*source_px)[i]);
            py->push_back((*source_py)[i]);
            pz->push_back((*source_pz)[i]);
          }
        }
        neutron_tree->Fill();
        return true;
      });
    box_file.Close();
  }

  out.cd();

  neutron_tree->Write("", TObject::kOverwrite);

  auto saved_neutron_count = get_or_make_object<TNamed>(&out, "NEUTRON_COUNT", "NEUTRON_COUNT", "0");
  saved_neutron_count->SetTitle(std::to_string(neutron_count + std::stoul(saved_neutron_count->GetTitle())).c_str());
  saved_neutron_count->Write();

  auto saved_event_count = get_or_make_object<TNamed>(&out, "EVENT_COUNT", "EVENT_COUNT", "0");
  saved_event_count->SetTitle(std::to_string(event_count + std::stoul(saved_event_count->GetTitle())).c_str());
  saved_event_count->Write();

  out.Close();
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */ ///////////////////////////////////////////////////////////////

//__Analyis of Cosmic Rays______________________________________________________________________
void analysis(const char* input,
              const char* output,
              bool update) {
  MATHUSLA::MU::neutron_analysis(input, output, update);
}
//----------------------------------------------------------------------------------------------

int main() {
  return 0;
}
