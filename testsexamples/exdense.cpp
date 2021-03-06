// Copyright (c) 2016 Idiap Research Institute, http://www.idiap.ch/
// Written by James Newling <jnewling@idiap.ch>

#include <iostream>
#include <map>
#include <vector>
#include <zentas/fasta.hpp>
#include <zentas/zentas.hpp>
#include <zentas/zentasinfo.hpp>

/* Test case : clustering dense vectors.
 * If in doubt as to what parameters to use for your application, just copy the ones here */
template <typename TFloat>
int cluster_dense()
{

  // generating random data
  size_t              ndata     = 100000;
  size_t              dimension = 5;
  std::vector<TFloat> data(ndata * dimension);
  srand(time(NULL));
  for (size_t i = 0; i < data.size(); ++i)
  {
    data[i] = (static_cast<TFloat>(rand() % 1000000)) / TFloat(1000000);
  }

  data[dimension * ndata / 2] = 1000000.;

  // set initialising medoid indices to be {0,...,K-1}
  size_t              K = 200;
  std::vector<size_t> indices_init(K);
  for (size_t i = 0; i < K; ++i)
  {
    indices_init[i] = i;
  }

  std::string initialisation_method = "uniform";  //"from_indices_init";

  // set algorithm and level of acceleration. For best performance (speed), this should *always* be
  // clarans at level 3.
  std::string algorithm = "clarans";
  size_t      level     = 3;

  // only relevent for clarans : max number of consecutive rejected proposals before halting. Make
  // it v-large if you don't want this to be your stopping criterion. note: if patient is true,
  // this is not used (see python function string in pyzentas.pyx for more details)
  size_t max_proposals = 20000;

  // if capture_output = true, rather than sending the run time info to terminal it is piped to
  // string text.
  size_t      capture_output = false;
  std::string text;

  // random seed, for proposal generation
  size_t seed = rand() % 1000;

  // maximum allowed time in seconds. make this v-large if you don't want this to be your stopping
  // criterion
  double max_time = 20.;

  // if the mean energy drops below this, stop and return. make this 0 if you don't want this to be
  // your stopping criterion
  double min_mE = 0.;

  // maximum ratio of times between initialisation (like k-means++) and running of k-medoids.
  double max_itok = 100.0;

  // save the final results (center indices and assignments) to these:
  std::vector<size_t> indices_final(K);
  std::vector<size_t> labels(ndata);

  // what metric to use. For metric data, this is one of l0, l1, l2 and li (infinity norm)
  std::string metric = "l2";

  // number of threads to use. Note for deterministic results this should be 1, otherwise order of
  // thread operations can change redistriubtion
  size_t nthreads = 1;

  // max number of rounds. For clarans, this is number of successful swaps. If you don't want this
  // to be your stopping criterion, make it v-large
  size_t max_rounds = 1000;

  // relevent for clarans : if false, implement a good swap as soon as it is found. If true
  // (recommended), if the time spent evaluating proposals is less than the time spent implementing
  // swaps, then keep searching for good swaps, only implementing a swap when you've spent as much
  // time looking as implementing. Motivation for this is that it doesn't make sense to spend the
  // majority of time implementing swaps, should spend at least half the time looking for good
  // swaps.
  bool patient = false;

  // energy can be log, identity, quadratic, cubic, exp, squarepotential.
  std::string energy = "identity";

  // rooted is purely an implementation issue, but can be important. Two version : if rooted =
  // true, data does not move around, so as to stay contiguous by cluster. if rooted = false, all
  // data is contiguous by cluster. Contiguity by cluster means faster memory access, but there is
  // the cost of moving data around. In general, rooted = false is fastest. Exceptions are sequence
  // data (and sparse) where the sequences vary greatly in length : for sequence data with rooted =
  // false, our implementations requires that each sequence is allocated the memory required by the
  // longest sequence. for rooted = true however the sequences are stored compactly (no gaps).
  // Summary : use rooted = false except for sequence data, where the sequences of of greatly
  // varying length.
  bool rooted = false;

  // only relevant if energy is squarepotential : squarepotential(d) = 1 if d > (>=? need to check)
  // criticial_radius, otherwise 0.
  double critical_radius = 0;

  // only relevent of energy is exponential.
  double exponent_coeff = 0;

  bool with_tests = false;

  bool do_vdimap = false;

  bool do_refinement = false;

  std::string rf_alg = "none";

  size_t rf_max_rounds = 0;
  double rf_max_time   = 0;

  bool do_balance_labels = false;

  // and finally, we cluster.
  nszen::vzentas<TFloat>(ndata,
                         dimension,
                         data.data(),
                         K,
                         nullptr,  // indices_init.data(),
                         initialisation_method,
                         algorithm,
                         level,
                         max_proposals,
                         capture_output,
                         text,
                         seed,
                         max_time,
                         min_mE,
                         max_itok,
                         indices_final.data(),
                         labels.data(),
                         metric,
                         nthreads,
                         max_rounds,
                         patient,
                         energy,
                         with_tests,
                         rooted,
                         critical_radius,
                         exponent_coeff,
                         do_vdimap,
                         do_refinement,
                         rf_alg,
                         rf_max_rounds,
                         rf_max_time,
                         do_balance_labels);

  // labels and indices_final have now been set, and can now used for the next step in your
  // application.

  return 0;
}

int cluster_dense_single() { return cluster_dense<float>(); }

int cluster_dense_double() { return cluster_dense<double>(); }

int main()
{
  // Choose your test and put it here (single precision or double precision).
  return cluster_dense_single();

  // std::cout << nszen::get_python_paramater_string() << std::endl;
  // std::cout << "\n\n" << nszen::get_python_function_decl() << std::endl;
}
