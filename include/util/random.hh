/*
 * include/util/random.hh
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTIL__RANDOM_HH
#define UTIL__RANDOM_HH
#pragma once

#include <random>

namespace MATHUSLA {

namespace util { namespace random { ////////////////////////////////////////////////////////////

//__Get Default Mersene Twister_________________________________________________________________
inline std::mt19937& mersene_twister() {
  static std::mt19937 mt(std::random_device{}());
  return mt;
}
//----------------------------------------------------------------------------------------------

//__Get New Mersene Twister_____________________________________________________________________
template<class Seed>
std::mt19937 new_mersene_twister(Seed&& seed) {
  std::mt19937 mt;
  mt.seed(seed);
  return mt;
}
//----------------------------------------------------------------------------------------------

//__Sample a Distribution Once__________________________________________________________________
template<class Distribution, class Generator>
const typename Distribution::result_type sample(Distribution&& dist,
                                                typename Distribution::param_type&& params,
                                                Generator&& gen) {
  if (dist.param() != params)
    dist.param(params);
  return dist(gen);
}
//----------------------------------------------------------------------------------------------

//__Sample a Distribution Many Times____________________________________________________________
template<class Distribution, class Generator>
const std::vector<typename Distribution::result_type> sample_many(std::size_t count,
                                                                  Distribution&& dist,
                                                                  typename Distribution::param_type&& params,
                                                                  Generator&& gen) {
  if (dist.param() != params)
    dist.param(params);
  std::vector<typename Distribution::result_type> out;
  out.reserve(count);
  for (std::size_t i{}; i < count; ++i)
    out.push_back(dist(gen));
  return out;
}
//----------------------------------------------------------------------------------------------

//__Sample a Uniform Distribution Once__________________________________________________________
template<class Generator=std::mt19937>
double uniform(double a = 0.0,
               double b = 1.0,
               Generator&& gen=std::forward<Generator>(mersene_twister())) {
  static std::uniform_real_distribution<> distribution(0, 1);
  using Dist = decltype(distribution);
  return sample(std::forward<Dist>(distribution), typename Dist::param_type{a, b}, std::forward<Generator>(gen));
}
//----------------------------------------------------------------------------------------------

//__Sample a Uniform Distribution Many Times____________________________________________________
template<class Generator=std::mt19937>
double uniform_vector(std::size_t n,
                      double a = 0.0,
                      double b = 1.0,
                      Generator&& gen=std::forward<Generator>(mersene_twister())) {
  static std::uniform_real_distribution<> distribution(0, 1);
  using Dist = decltype(distribution);
  return sample_many(n, std::forward<Dist>(distribution), typename Dist::param_type{a, b}, std::forward<Generator>(gen));
}
//----------------------------------------------------------------------------------------------

/* TODO:

template<class Transform, class Distribution, class Generator>
const Distribution::result_type transform_sample(Distribution&& dist,
                                                 Distribution::param_type&& params,
                                                 Generator&& gen,
                                                 Transform&& f) {
  return f(sample(dist, params, gen));
}
//----------------------------------------------------------------------------------------------

template<class Transform, class Distribution, class Generator>
const std::vector<Distribution::result_type> transform_sample_many(std::size_t count,
                                                                   Distribution&& dist,
                                                                   Distribution::param_type&& params,
                                                                   Generator&& gen,
                                                                   Transform&& f) {
  auto& sampled = sample_many(count, dist, params, gen);
  for (auto& x : sampled)
    x = f(x);
  return sampled;
}
//----------------------------------------------------------------------------------------------

*/

} } /* namespace util::random */ ///////////////////////////////////////////////////////////////

} /* namespace MATHUSLA */

#endif /* UTIL__RANDOM_HH */
