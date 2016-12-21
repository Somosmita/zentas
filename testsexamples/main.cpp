/*Copyright (c) 2016 Idiap Research Institute, http://www.idiap.ch/
Written by James Newling <jnewling@idiap.ch>

zentas is a k-medoids library written in C++ and Python. This file is part of zentas.
zentas is free software: you can redistribute it and/or modify it under the terms of
the GNU General Public License version 3 as published by the Free Software Foundation.
zentas is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details. You should have received a copy of
the GNU General Public License along with zentas. If not, see
<http://www.gnu.org/licenses/>.
*/
#include <vector>
#include <iostream>
#include <map>

#include "fasta.hpp"
#include "zentas.hpp"


/* Test case : clustering dense vectors. 
 * If in doubt as to want parameters to use, just copy these */
template <typename TFloat>
int cluster_dense(){
  //generating random data
  size_t ndata = 100000;
  size_t dimension = 4;
  std::vector<TFloat> data (ndata*dimension);
  for (size_t i = 0; i < data.size(); ++i){
    data[i] = (static_cast<float> (rand()%1000000)) / 1000000.  ;
  }
  
  //set initialising medoid indices to just be 0:K-1
  size_t K = 500;
  std::vector<size_t> indices_init (K);
  for (size_t i = 0; i < K; ++i){
    indices_init[i] = i;
  }
  
  //set algorithm and level of acceleration. This should *always* be clarans at level 3.
  std::string algorithm = "clarans";
  size_t level = 3;
  
  //only relevent for clarans : max number of consecutive rejected proposals before halting. Just make it large. 
  size_t max_proposals = 1000000;
  
  //if capture_output = true, rather than sending run time info to terminal it is piped to string text.
  size_t capture_output = false;
  std::string text;
  
  //random seed, for proposal generation
  size_t seed = 1011;

  //maximum allowed time in seconds
  double maxtime = 2.;
  
  //save the final results (center indices and assignments) to these
  std::vector<size_t> indices_final (K);
  std::vector<size_t> labels (ndata);
  
  //what metric to use. For metric data, this is one of l0, l1, l2 and li (infinity norm)
  std::string metric = "l1"; 
  
  //number of threads to use
  size_t nthreads = 4;
  
  //max number of rounds. For clarans, this is number of successful swaps
  size_t maxrounds = 1000000;

  //relevent for clarans : if false, implement a good swap as soon as found. If true (recommended), if the time spent evaluating proposals is less than the time spent implementing swaps, then keep searching for good swaps, only implement a swap when you've spent as much time looking as implementing. Motivation for this is that it doesn't make sense to spend the majority of time implementing swaps, should spend at least half the time looking for good swaps. 
  bool patient = true;
  
  //energy can be log, identity, quadratic, cubic, exp, squarepotential.
  std::string energy = "cubic";
  
  //rooted is purely an implementation issue, but can be important. Two version : if rooted = true, data does not move around so as to stay contiguous by cluster. if rooted = false, all data is contiguous by cluster. Contiguity by cluster means faster memory access, but there is the cost of moving data around. In general, rooted = false is fastest. Exceptions are sequence data (and sparse) where the sequences vary greatly in length : for sequence data with rooted = false, our implementations requires that each sequence is allocated the memory required by the longest sequence. for rooted = true however the sequences are stored compactly (no gaps). Summary : use rooted = false except for sequence data of greatly varying length. 
  bool rooted = false;
  
  //only relevant if energy is squarepotential : squarepotential(d) = 1 if d > (>=? need to check) criticial_radius, otherwise 0.
  double critical_radius = 0; 
  
  //only relevent of energy is exponential.
  double exponent_coeff = 0; 
  
  
  nszen::vzentas<TFloat>(ndata, dimension, data.data(), K, indices_init.data(), algorithm, level, max_proposals, capture_output, text, seed, maxtime, indices_final.data(), labels.data(), metric, nthreads, maxrounds, patient, energy, rooted, critical_radius, exponent_coeff);

  return 0;
  
}



/* Test case : clustering sparse vectors. */
int cluster_sparse(){
  
  //generating random data
  size_t ndata = 5;
  std::vector<size_t> sizes = {2,3,2,1,2};
  size_t n_non_zeros = 10; //sum of sizes. could be float.

  std::vector<double> data_in (n_non_zeros);   
  for (size_t i = 0; i < n_non_zeros; ++i){
    data_in[i] = 3.1415111101111;
  }
  
  std::vector<size_t> indices {
    3, 7,
    3,7,11,
    4, 12, 
    4, 
    4, 100000}; 
  
  size_t K = 2;
  std::vector<size_t> indices_init = {0,1};
  std::string algorithm = "clarans";
  size_t level = 0;
  size_t max_proposals = 100;
  bool capture_output = false;
  std::string text;
  size_t seed = 1011;
  double maxtime = 0.5;
  std::vector<size_t> indices_final (K);
  std::vector<size_t> labels (ndata);
  std::string metric = "l1";
  std::string energy = "identity";
  size_t nthreads = 1;
  size_t maxrounds = 100;
  bool patient = true;
  bool rooted = true;
  double critical_radius = 0;
  double exponent_coeff = 0;
 

  nszen::sparse_vector_zentas(ndata, sizes.data(), data_in.data(), indices.data(), K, indices_init.data(), algorithm, level, max_proposals,  capture_output, text, seed, maxtime, indices_final.data(), labels.data(), metric, nthreads, maxrounds, patient, energy, rooted, critical_radius, exponent_coeff);
  
  std::cout << std::endl;
  for (size_t i = 0; i < ndata; ++i){
    std::cout << labels[i] << std::endl;
  }

  return 0;
}


int cluster_dense_single(){
  return cluster_dense<float>();
}

int cluster_dense_double(){
  return cluster_dense<double>();
}


/* Test case : word clustering. */
int cluster_words(){
  
  std::vector<std::string> filenames; 
  /* this path may need changing */
  std::string root = "/idi";
  root = root + "ap/home/jnew" + "ling/libraries/zentas/data/";  
  //filenames.push_back(root + "nucleic.txt");
  filenames.push_back(root + "words1.txt");
  filenames.push_back(root + "words2.txt");
  std::string outfilename = root + "output.txt";  
  std::string  costfilename = root + "costs.txt";
  size_t K = 3;
  std::string algorithm("clarans");
  size_t level = 3;
  size_t max_proposals = 10000;
  size_t capture_output = false;
  std::string text;
  size_t seed = 1011;
  double maxtime = 10000.;
  std::string metric("normalised levenshtein");
  size_t nthreads = 4;
  size_t maxrounds = 1000;
  bool patient = false;
  std::string energy("identity");
  bool rooted = true;
  double critical_radius = 0;
  double exponent_coeff = 0;
  nszen::textfilezentas(filenames, outfilename, costfilename, K, algorithm, level, max_proposals, capture_output, text, seed, maxtime, metric, nthreads, maxrounds, patient, energy, rooted, critical_radius, exponent_coeff);
 
  return 0; 
}

int main(){
  //Choose your test and put it here. 
  return cluster_sparse();
}