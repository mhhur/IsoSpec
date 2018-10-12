/*
 *   Copyright (C) 2015-2018 Mateusz Łącki and Michał Startek.
 *
 *   This file is part of IsoSpec.
 *
 *   IsoSpec is free software: you can redistribute it and/or modify
 *   it under the terms of the Simplified ("2-clause") BSD licence.
 *
 *   IsoSpec is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *   You should have received a copy of the Simplified BSD Licence
 *   along with IsoSpec.  If not, see <https://opensource.org/licenses/BSD-2-Clause>.
 */


#include <cmath>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <tuple>
#include <unordered_map>
#include <queue>
#include <utility>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <stdexcept>
#include <string>
#include <limits>
#include <assert.h>
#include "platform.h"
#include "conf.h"
#include "dirtyAllocator.h"
#include "operators.h"
#include "summator.h"
#include "marginalTrek++.h"
#include "isoSpec++.h"
#include "misc.h"
#include "element_tables.h"


using namespace std;

namespace IsoSpec
{

Iso::Iso(
    int             _dimNumber,
    const int*      _isotopeNumbers,
    const int*      _atomCounts,
    const double* const *  _isotopeMasses,
    const double* const *  _isotopeProbabilities
) :
disowned(false),
dimNumber(_dimNumber),
isotopeNumbers(array_copy<int>(_isotopeNumbers, _dimNumber)),
atomCounts(array_copy<int>(_atomCounts, _dimNumber)),
confSize(_dimNumber * sizeof(int)),
allDim(0),
marginals(nullptr),
modeLProb(0.0)
{
	setupMarginals(_isotopeMasses, _isotopeProbabilities);
}

Iso::Iso(Iso&& other) :
disowned(other.disowned),
dimNumber(other.dimNumber),
isotopeNumbers(other.isotopeNumbers),
atomCounts(other.atomCounts),
confSize(other.confSize),
allDim(other.allDim),
marginals(other.marginals),
modeLProb(other.modeLProb)
{
    other.disowned = true;
}


Iso::Iso(const Iso& other, bool fullcopy) :
disowned(fullcopy ? throw std::logic_error("Not implemented") : true),
dimNumber(other.dimNumber),
isotopeNumbers(fullcopy ? array_copy<int>(other.isotopeNumbers, dimNumber) : other.isotopeNumbers),
atomCounts(fullcopy ? array_copy<int>(other.atomCounts, dimNumber) : other.atomCounts),
confSize(other.confSize),
allDim(other.allDim),
marginals(fullcopy ? throw std::logic_error("Not implemented") : other.marginals),
modeLProb(other.modeLProb)
{}


inline void Iso::setupMarginals(const double* const * _isotopeMasses, const double* const * _isotopeProbabilities)
{
    if (marginals == nullptr)
    {
        marginals = new Marginal*[dimNumber];
        for(int i=0; i<dimNumber;i++)
        {
        allDim += isotopeNumbers[i];
        marginals[i] = new Marginal(
                _isotopeMasses[i],
                _isotopeProbabilities[i],
                isotopeNumbers[i],
                atomCounts[i]
            );
            modeLProb += marginals[i]->getModeLProb();
        }
    }

}

Iso::~Iso()
{
    if(!disowned)
    {
    if (marginals != nullptr)
        dealloc_table(marginals, dimNumber);
    delete[] isotopeNumbers;
    delete[] atomCounts;
    }
}


double Iso::getLightestPeakMass() const
{
    double mass = 0.0;
    for (int ii=0; ii<dimNumber; ii++)
        mass += marginals[ii]->getLightestConfMass();
    return mass;
}

double Iso::getHeaviestPeakMass() const
{
    double mass = 0.0;
    for (int ii=0; ii<dimNumber; ii++)
        mass += marginals[ii]->getHeaviestConfMass();
    return mass;
}



inline int str_to_int(const string& s)
{
    char* endptr[1];
    const char* c_s = s.c_str();
    int ret = (int) strtol(c_s, endptr, 10);
    if (c_s == endptr[0])
        throw invalid_argument("Invalid formula");
    return ret;
}

Iso::Iso(const char* formula) :
disowned(false),
allDim(0),
marginals(nullptr),
modeLProb(0.0)
{
    std::vector<const double*> isotope_masses;
    std::vector<const double*> isotope_probabilities;

    dimNumber = parse_formula(formula, isotope_masses, isotope_probabilities, &isotopeNumbers, &atomCounts, &confSize);

    setupMarginals(isotope_masses.data(), isotope_probabilities.data());
}

unsigned int parse_formula(const char* formula, std::vector<const double*>& isotope_masses, std::vector<const double*>& isotope_probabilities, int** isotopeNumbers, int** atomCounts, unsigned int* confSize)
{
    // This function is NOT guaranteed to be secure against malicious input. It should be used only for debugging.
    string cpp_formula(formula);
    int last_modeswitch = 0;
    int mode = 0;
    int pos = 0;
    std::vector<string> elements;
    std::vector<int> numbers;
    while(formula[pos] != '\0')
    {
        if(isdigit(formula[pos]) && mode == 0)
        {
            elements.push_back(cpp_formula.substr(last_modeswitch, pos-last_modeswitch));
            last_modeswitch = pos;
            mode = 1;
        }
        else if(isalpha(formula[pos]) && mode == 1)
        {
            numbers.push_back(str_to_int(cpp_formula.substr(last_modeswitch, pos-last_modeswitch)));
            last_modeswitch = pos;
            mode = 0;
        }
        pos++;
    }

    numbers.push_back(str_to_int(cpp_formula.substr(last_modeswitch, pos)));


    if(elements.size() != numbers.size())
        throw invalid_argument("Invalid formula");

    std::vector<int> element_indexes;

    for (unsigned int i=0; i<elements.size(); i++)
    {
        int idx = -1;
        for(int j=0; j<ISOSPEC_NUMBER_OF_ISOTOPIC_ENTRIES; j++)
        {
            if (elements[i].compare(elem_table_symbol[j]) == 0)
            {
                idx = j;
                break;
            }
        }
        if(idx < 0)
            throw invalid_argument("Invalid formula");
        element_indexes.push_back(idx);

    }

    vector<int> _isotope_numbers;

    for(vector<int>::iterator it = element_indexes.begin(); it != element_indexes.end(); ++it)
    {
        int num = 0;
        int at_idx = *it;
        int atomicNo = elem_table_atomicNo[at_idx];
        while(at_idx < ISOSPEC_NUMBER_OF_ISOTOPIC_ENTRIES && elem_table_atomicNo[at_idx] == atomicNo)
        {
            at_idx++;
            num++;
        }
        _isotope_numbers.push_back(num);
    }

    for(vector<int>::iterator it = element_indexes.begin(); it != element_indexes.end(); ++it)
    {
        isotope_masses.push_back(&elem_table_mass[*it]);
        isotope_probabilities.push_back(&elem_table_probability[*it]);
    };

    const unsigned int dimNumber = elements.size();

    *isotopeNumbers = array_copy<int>(_isotope_numbers.data(), dimNumber);
    *atomCounts = array_copy<int>(numbers.data(), dimNumber);
    *confSize = dimNumber * sizeof(int);

    return dimNumber;

}


/*
 * ----------------------------------------------------------------------------------------------------------
 */



IsoGenerator::IsoGenerator(Iso&& iso, bool alloc_partials) :
    Iso(std::move(iso)),
    partialLProbs(alloc_partials ? new double[dimNumber+1] : nullptr),
    partialMasses(alloc_partials ? new double[dimNumber+1] : nullptr),
    partialExpProbs(alloc_partials ? new double[dimNumber+1] : nullptr)
{
    if(alloc_partials)
    {
        partialLProbs[dimNumber] = 0.0;
        partialMasses[dimNumber] = 0.0;
        partialExpProbs[dimNumber] = 1.0;
    }
}


IsoGenerator::~IsoGenerator() 
{
    if(partialLProbs != nullptr)
        delete[] partialLProbs; 
    if(partialMasses != nullptr)
        delete[] partialMasses; 
    if(partialExpProbs != nullptr)
        delete[] partialExpProbs; 
}



/*
 * ----------------------------------------------------------------------------------------------------------
 */






IsoThresholdGenerator::IsoThresholdGenerator(Iso&& iso, double _threshold, bool _absolute, int tabSize, int hashSize)
: IsoGenerator(std::move(iso)),
Lcutoff(_threshold <= 0.0 ? std::numeric_limits<double>::lowest() : (_absolute ? log(_threshold) : log(_threshold) + modeLProb))
{
    counter = new int[dimNumber];
    maxConfsLPSum = new double[dimNumber-1];
    marginalResults = new PrecalculatedMarginal*[dimNumber];

    bool empty = false;

    for(int ii=0; ii<dimNumber; ii++)
    {
        counter[ii] = 0;
        marginalResults[ii] = new PrecalculatedMarginal(std::move(*(marginals[ii])),
                                                        Lcutoff - modeLProb + marginals[ii]->getModeLProb(),
                                                        true,
                                                        tabSize,
                                                        hashSize);

        if(!marginalResults[ii]->inRange(0))
            empty = true;
    }

    if(dimNumber > 1)
        maxConfsLPSum[0] = marginalResults[0]->getModeLProb();

    for(int ii=1; ii<dimNumber-1; ii++)
        maxConfsLPSum[ii] = maxConfsLPSum[ii-1] + marginalResults[ii]->getModeLProb();

    if(!empty)
    {
        recalc(dimNumber-1);
        counter[0]--;
    }
    else
        terminate_search();


}

bool IsoThresholdGenerator::advanceToNextConfiguration()
{
    counter[0]++;
    partialLProbs[0] = partialLProbs[1] + marginalResults[0]->get_lProb(counter[0]);
    if(partialLProbs[0] >= Lcutoff)
    {
        partialMasses[0] = partialMasses[1] + marginalResults[0]->get_mass(counter[0]);
        partialExpProbs[0] = partialExpProbs[1] * marginalResults[0]->get_eProb(counter[0]);
        return true;
    }

    // If we reached this point, a carry is needed

    int idx = 0;

    while(idx<dimNumber-1)
    {
        counter[idx] = 0;
        idx++;
        counter[idx]++;
        partialLProbs[idx] = partialLProbs[idx+1] + marginalResults[idx]->get_lProb(counter[idx]);
        if(partialLProbs[idx] + maxConfsLPSum[idx-1] >= Lcutoff)
        {
            partialMasses[idx] = partialMasses[idx+1] + marginalResults[idx]->get_mass(counter[idx]);
            partialExpProbs[idx] = partialExpProbs[idx+1] * marginalResults[idx]->get_eProb(counter[idx]);
            recalc(idx-1);
            return true;
        }
    }

    terminate_search();
    return false;
}

void IsoThresholdGenerator::terminate_search()
{
    for(int ii=0; ii<dimNumber; ii++)
        counter[ii] = marginalResults[ii]->get_no_confs();
}

/*
 * ------------------------------------------------------------------------------------------------------------------------
 */

IsoOrderedGenerator::IsoOrderedGenerator(Iso&& iso, int _tabSize, int _hashSize) :
IsoGenerator(std::move(iso), false), allocator(dimNumber, _tabSize)
{
    partialLProbs = &currentLProb;
    partialMasses = &currentMass;
    partialExpProbs = &currentEProb;

    marginalResults = new MarginalTrek*[dimNumber];

    for(int i = 0; i<dimNumber; i++)
        marginalResults[i] = new MarginalTrek(std::move(*(marginals[i])), _tabSize, _hashSize);

    logProbs        = new const vector<double>*[dimNumber];
    masses          = new const vector<double>*[dimNumber];
    marginalConfs   = new const vector<int*>*[dimNumber];

    for(int i = 0; i<dimNumber; i++)
    {
        masses[i] = &marginalResults[i]->conf_masses();
        logProbs[i] = &marginalResults[i]->conf_lprobs();
        marginalConfs[i] = &marginalResults[i]->confs();
    }

    topConf = allocator.newConf();
    memset(
            reinterpret_cast<char*>(topConf) + sizeof(double),
            0,
            sizeof(int)*dimNumber
    );

    *(reinterpret_cast<double*>(topConf)) =
    combinedSum(
                getConf(topConf),
                logProbs,
                dimNumber
    );

    pq.push(topConf);

}


IsoOrderedGenerator::~IsoOrderedGenerator()
{
    dealloc_table<MarginalTrek*>(marginalResults, dimNumber);
    delete[] logProbs;
    delete[] masses;
    delete[] marginalConfs;
    partialLProbs = nullptr;
    partialMasses = nullptr;
    partialExpProbs = nullptr;
}


bool IsoOrderedGenerator::advanceToNextConfiguration()
{
    if(pq.size() < 1)
        return false;


    topConf = pq.top();
    pq.pop();

    int* topConfIsoCounts = getConf(topConf);

    currentLProb = *(reinterpret_cast<double*>(topConf));
    currentMass = combinedSum( topConfIsoCounts, masses, dimNumber );
    currentEProb = exp(currentLProb);

    ccount = -1;
    for(int j = 0; j < dimNumber; ++j)
    {
        if(marginalResults[j]->probeConfigurationIdx(topConfIsoCounts[j] + 1))
        {
            if(ccount == -1)
            {
                topConfIsoCounts[j]++;
                *(reinterpret_cast<double*>(topConf)) = combinedSum(topConfIsoCounts, logProbs, dimNumber);
                pq.push(topConf);
                topConfIsoCounts[j]--;
                ccount = j;
            }
            else
            {
                void* acceptedCandidate = allocator.newConf();
                int* acceptedCandidateIsoCounts = getConf(acceptedCandidate);
                memcpy(acceptedCandidateIsoCounts, topConfIsoCounts, confSize);

                acceptedCandidateIsoCounts[j]++;

                *(reinterpret_cast<double*>(acceptedCandidate)) = combinedSum(acceptedCandidateIsoCounts, logProbs, dimNumber);

                pq.push(acceptedCandidate);
            }
        }
        if(topConfIsoCounts[j] > 0)
            break;
    }
    if(ccount >=0)
        topConfIsoCounts[ccount]++;

    return true;
}



/*
 * ---------------------------------------------------------------------------------------------------
 */




#if !ISOSPEC_BUILDING_R

void printConfigurations(
    const   std::tuple<double*,double*,int*,int>& results,
    int     dimNumber,
    int*    isotopeNumbers
){
    int m = 0;

    for(int i=0; i<std::get<3>(results); i++){

        std::cout << "Mass = "  << std::get<0>(results)[i] <<
        "\tand log-prob = "     << std::get<1>(results)[i] <<
        "\tand prob = "                 << exp(std::get<1>(results)[i]) <<
        "\tand configuration =\t";


        for(int j=0; j<dimNumber; j++){
            for(int k=0; k<isotopeNumbers[j]; k++ )
            {
                std::cout << std::get<2>(results)[m] << " ";
                m++;
            }
            std::cout << '\t';
        }


        std::cout << std::endl;
    }
}

#endif /* !ISOSPEC_BUILDING_R */



IsoLayeredGenerator::IsoLayeredGenerator( Iso&&     iso,
                        double    _targetCoverage,
                        double    _percentageToExpand,
                        int       _tabSize,
                        int       _hashSize,
                        bool      trim
) : IsoGenerator(std::move(iso)),
allocator(dimNumber, _tabSize),
candidate(new int[dimNumber]),
targetCoverage(_targetCoverage),
percentageToExpand(_percentageToExpand),
do_trim(trim),
layers(0),
generator_position(-1)
{
    marginalResults = new MarginalTrek*[dimNumber];

    for(int i = 0; i<dimNumber; i++)
        marginalResults[i] = new MarginalTrek(std::move(*(marginals[i])), _tabSize, _hashSize);

    logProbs        = new const vector<double>*[dimNumber];
    masses          = new const vector<double>*[dimNumber];
    marginalConfs   = new const vector<int*>*[dimNumber];

    for(int i = 0; i<dimNumber; i++)
    {
        masses[i] = &marginalResults[i]->conf_masses();
        logProbs[i] = &marginalResults[i]->conf_lprobs();
        marginalConfs[i] = &marginalResults[i]->confs();
    }

    void* topConf = allocator.newConf();
    bzero(reinterpret_cast<char*>(topConf) + sizeof(double), sizeof(int)*dimNumber);
    *(reinterpret_cast<double*>(topConf)) = combinedSum(getConf(topConf), logProbs, dimNumber);

    current = new std::vector<void*>();
    next    = new std::vector<void*>();

    current->push_back(topConf);

    lprobThr = (*reinterpret_cast<double*>(topConf));

    while(advanceToNextLayer()) {};
}


IsoLayeredGenerator::~IsoLayeredGenerator()
{
    if(current != nullptr)
        delete current;
    if(next != nullptr)
        delete next;
    delete[] logProbs;
    delete[] masses;
    delete[] marginalConfs;
    delete[] candidate;
    dealloc_table(marginalResults, dimNumber);
}

bool IsoLayeredGenerator::advanceToNextLayer()
{
    layers += 1;
    double maxFringeLprob = -std::numeric_limits<double>::infinity();

    if(current == nullptr)
        return false;
    int accepted_in_this_layer = 0;
    Summator prob_in_this_layer(totalProb);

    void* topConf;

    while(current->size() > 0)
    {
        topConf = current->back();
        current->pop_back();

        double top_lprob = getLProb(topConf);

        if(top_lprob >= lprobThr)
        {
#ifdef DEBUG
            hits += 1;
#endif /* DEBUG */
            newaccepted.push_back(topConf);
            accepted_in_this_layer++;
            prob_in_this_layer.add(exp(top_lprob));
        }
        else
        {
#ifdef DEBUG
            moves += 1;
#endif /* DEBUG */
            next->push_back(topConf);
            continue;
        }

        int* topConfIsoCounts = getConf(topConf);

        for(int j = 0; j < dimNumber; ++j)
        {
            // candidate cannot refer to a position that is
            // out of range of the stored marginal distribution.
            if(marginalResults[j]->probeConfigurationIdx(topConfIsoCounts[j] + 1))
            {
                memcpy(candidate, topConfIsoCounts, confSize);
                candidate[j]++;

                void*       acceptedCandidate          = allocator.newConf();
                int*        acceptedCandidateIsoCounts = getConf(acceptedCandidate);
                memcpy(     acceptedCandidateIsoCounts, candidate, confSize);

                double newConfProb = combinedSum(
                    candidate,
                    logProbs,
                    dimNumber
                );



                *(reinterpret_cast<double*>(acceptedCandidate)) = newConfProb;

                if(newConfProb >= lprobThr)
                    current->push_back(acceptedCandidate);
                else
        {
                    next->push_back(acceptedCandidate);
            if(newConfProb > maxFringeLprob)
                maxFringeLprob = top_lprob;
        }
            }
            if(topConfIsoCounts[j] > 0)
                break;
        }
    }

    if(next == nullptr || next->size() < 1)
        return false;
    else
    {
        if(prob_in_this_layer.get() < targetCoverage)
        {
#ifdef DEBUG
            Summator testDupa(prob_in_this_layer);
            for (std::vector<void*>::iterator it = next->begin(); it != next->end(); it++) {
                testDupa.add(exp(getLProb(*it)));
            }
            std::cout << "Prob(Layer) = " << prob_in_this_layer.get() << std::endl;
            std::cout << "Prob(Layer)+Prob(Fringe) = " << testDupa.get() << std::endl;
            std::cout << "Layers = " << layers << std::endl;
            std::cout << std::endl;
#endif /* DEBUG */

        // // This was an attempt to merge two methods: layered and layered_estimating
        // // that does not work so good as predicted.
//             if( estimateThresholds and ( prob_in_this_layer.get() >= targetCoverage*.99 ) ){
//                 estimateThresholds = false;
//                 percentageToExpand = .25; // The ratio of one rectangle to the rectangle.
// #ifdef DEBUG
//                 std::cout << "We switch!" << std::endl;
// #endif /* DEBUG */
//             }

#ifdef DEBUG
                std::cout << "percentageToExpand = " << percentageToExpand << std::endl;
#endif /* DEBUG */

            std::vector<void*>* nnew = current;
            nnew->clear();
            current = next;
            next = nnew;
            int howmany = floor(current->size()*percentageToExpand);
            if(estimateThresholds){
                // Screw numeric correctness, ARRRRRRR!!! Well, this is an estimate anyway, doesn't have to be that precise
                // lprobThr += log((1.0-targetCoverage))+log1p((percentageToExpand-1.0)/layers) - log(1.0-prob_in_this_layer.get());
                lprobThr += log(1.0-targetCoverage) + log(1.0-(1.0-percentageToExpand)/pow(layers, 2.0)) - log(1.0 -prob_in_this_layer.get());
                if(lprobThr > maxFringeLprob){
                    lprobThr = maxFringeLprob;
                    estimateThresholds = false;
                    percentageToExpand = .3;
#ifdef DEBUG
                    std::cout << "We switch to other method because density estimates where higher than max on fringe." << std::endl;
#endif /* DEBUG */
                    lprobThr = getLProb(quickselect(current->data(), howmany, 0, current->size()));
                }
            } else
                lprobThr = getLProb(quickselect(current->data(), howmany, 0, current->size()));
            totalProb = prob_in_this_layer;
        }
        else
        {
#ifdef DEBUG
            std::cerr << "No. layers: " << layers << "  hits: " << hits << "    misses: " << moves << " miss ratio: " << static_cast<double>(moves) / static_cast<double>(hits) << std::endl;
#endif /* DEBUG */
            delete next;
            next = nullptr;
            delete current;
            current = nullptr;
            int start = 0;
            int end = accepted_in_this_layer - 1;
            void* swapspace;

            if(do_trim)
            {
                void** lastLayer = &(newaccepted.data()[newaccepted.size()-accepted_in_this_layer]);

                Summator qsprob(totalProb);
                while(totalProb.get() < targetCoverage)
                {
                    if(start == end)
                        break;

                    // Partition part

                    int len = end - start;
#ifdef BUILDING_R
            int pivot = len/2 + start;  // We're very definitely NOT switching to R to use a RNG, and if R sees us use C RNG it complains...
#else
            int pivot = rand() % len + start;
#endif
                    void* pval = lastLayer[pivot];
                    double pprob = getLProb(pval);
                    mswap(lastLayer[pivot], lastLayer[end-1]);
                    int loweridx = start;
                    for(int i=start; i<end-1; i++)
                    {
                        if(getLProb(lastLayer[i]) > pprob)
                        {
                            mswap(lastLayer[i], lastLayer[loweridx]);
                            loweridx++;
                        }
                    }
                    mswap(lastLayer[end-1], lastLayer[loweridx]);

                    // Selection part

                    Summator leftProb(qsprob);
                    for(int i=start; i<=loweridx; i++)
                    {
                        leftProb.add(exp(getLProb(lastLayer[i])));
                    }
                    if(leftProb.get() < targetCoverage)
                    {
                        start = loweridx+1;
                        qsprob = leftProb;
                    }
                    else
                        end = loweridx;
                }
            int accend = newaccepted.size()-accepted_in_this_layer+start+1;
    #ifdef DEBUG
                std::cerr << "Last layer size: " << accepted_in_this_layer << " Total size: " << newaccepted.size() << "    Total size after trimming: " << accend << " No. trimmed: " << -start-1+accepted_in_this_layer
            << "    Trimmed to left ratio: " << static_cast<double>(-start-1+accepted_in_this_layer) / static_cast<double>(accend) << std::endl;
    #endif /* DEBUG */

                totalProb = qsprob;
                newaccepted.resize(accend);
                return true;
            }
            else // No trimming
            {
                totalProb = prob_in_this_layer;
                return true;
            }
        }
    }
    return true;

}

bool IsoLayeredGenerator::advanceToNextConfiguration()
{
    generator_position++;
    if(generator_position < newaccepted.size())
    {
        partialLProbs[0] = getLProb(newaccepted[generator_position]);
        partialMasses[0] = combinedSum(getConf(newaccepted[generator_position]), masses, dimNumber);
        partialExpProbs[0] = exp(partialLProbs[0]);
        return true;
    }
    else
        return false;
}




} // namespace IsoSpec

