#include <iostream>

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <limits>
#include <algorithm>
#include <functional>

using namespace std;

#include "kmer32.h"
#include "kmerXX.h"

#if maxK > 32 // store k-mers in a bitset, allows larger k-mers
    typedef kmerXX kmer;
    typedef bitset<2*maxK> kmer_t;
#else // store k-mer bits in an integer, optimizes performance
    typedef kmer32 kmer;
    typedef uint64_t kmer_t;
#endif

#include "color64.h"
#include "colorXX.h"

#if maxN > 64 // store colors in a bitset, allows more input files
    typedef colorXX color;
    typedef bitset<maxN> color_t;
#else // store color bits in an integer, optimizes performance
    typedef color64 color;
    typedef uint64_t color_t;
#endif
	
/**
 * A tree structure that is needed for generating a NEWICK string.
 */
struct set { 
	color_t taxa;
	double weight;
	vector<set *> subsets;
}; 


/**
 * This class manages the k-mer/color hash tables and split list.
 */
class graph {

private:

    /**
     * This is a hash table mapping k-mers to colors [O(1)].
     */
    static unordered_map<kmer_t, color_t> kmer_table;

    /**
     * This is a hash table mapping colors to weights [O(1)].
     */
    static unordered_map<color_t, array<uint32_t,2>> color_table;
	
	/**
	 * initializes a new set struct.
     * @param taxa color_t coding all taxa beneath this node
     * @param subsets list of subsets
	 */
// 	static struct set* newSet(color_t taxa, vector<set *> subsets);

public:
	
  

    /**
     * This is an ordered tree collecting the splits [O(log n)].
     */
    static multimap<double, color_t, greater<>> split_list;

    /**
     * This is the size of the top list.
     */
    static uint64_t t;

    /**
     * This function initializes the top list size.
     *
     * @param t top list size
     */
    static void init(uint64_t& top_size);

    /**
     * This function extracts k-mers from a sequence and adds them to the hash table.
     *
     * @param str dna sequence
     * @param color color flag
     * @param reverse merge complements
     */
    static void add_kmers(string& str, uint64_t& color, bool& reverse);

    /**
     * This function extracts k-mers from a sequence and adds them to the hash table.
     *
     * @param str dna sequence
     * @param color color flag
     * @param reverse merge complements
     * @param max_iupac allowed number of ambiguous k-mers per position
     */
    static void add_kmers(string& str, uint64_t& color, bool& reverse, uint64_t& max_iupac);

    /**
     * This function iterates over the hash table and calculates the split weights.
     *
     * @param mean weight function
     * @param verbose print progress
     */
    static void add_weights(double mean(uint32_t&, uint32_t&), bool& verbose);

    /**
     * This function adds a single split (weight and colors) to the output list.
     *
     * @param weight split weight
     * @param color split colors
     */
    static void add_split(double& weight, color_t& color);

    /**
     * This function filters a greedy maximum weight tree compatible subset.
     *
     * @param verbose print progress
     */
    static void filter_strict(bool& verbose);

	/**
	 * This function filters a greedy maximum weight tree compatible subset and returns a newick string.
     *
	 * @param map function that maps an integer to the original id, or null
	 * @param verbose print progress
	 */
	static string filter_strict(std::function<string(const uint64_t&)> map, bool& verbose);
	
 	
    /**
     * This function filters a greedy maximum weight weakly compatible subset.
     *
     * @param verbose print progress
     */
    static void filter_weakly(bool& verbose);

	/**
     * This function filters a greedy maximum weight n-tree compatible subset.
     *
     * @param n number of trees
     * @param verbose print progress
     */
    static void filter_n_tree(uint64_t n, bool& verbose);

   /**
     * This function filters a greedy maximum weight n-tree compatible subset and returns a string with all trees in newick format.
     *
     * @param n number of trees
	 * @param map function that maps an integer to the original id, or null
     * @param verbose print progress
     */
    static string filter_n_tree(uint64_t n, std::function<string(const uint64_t&)> map, bool& verbose);


protected:

    /**
     * This function tests if a split is compatible with an existing set of splits.
     *
     * @param color new split
     * @param color_set set of splits
     * @return true, if compatible
     */
    static bool test_strict(color_t& color, vector<color_t>& color_set);

    /**
     * This function tests if a split is weakly compatible with an existing set of splits.
     *
     * @param color new split
     * @param color_set set of splits
     * @return true, if weakly compatible
     */
    static bool test_weakly(color_t& color, vector<color_t>& color_set);

    /**
     * This function calculates the multiplicity of iupac k-mers.
     *
     * @param product overall multiplicity
     * @param factors per base multiplicity
     * @param input iupac character
     */
    static void iupac_calc(long double& product, vector<uint8_t>& factors, char& input);

    /**
     * This function shifts a base into a set of ambiguous iupac k-mers.
     *
     * @param prev set of k-mers
     * @param next set of k-mers
     * @param input iupac character
     */
    static void iupac_shift(unordered_set<kmer_t>& prev, unordered_set<kmer_t>& next, char& input);
	
	/**
	* This function returns a tree structure (struct set) generated from the given list of color sets
	*
	* @param color_set list of color sets
	* @return tree structure (struct set)
	*/
	static set * build_tree(vector<color_t>& color_set);

	/**
	 * This function recursively refines a given set/tree structure by a given split
	 *
	 * @param current_set node of currently considered (sub-)set/tree structure
	 * @param split color set to refine by
	 * @return whether or not the given split is compatible with the set/tree structure
	 */
	static bool refine_tree(set * current_set, color_t& split, color_t& allTaxa);
	
	/**
	* This function returns a newick string generated from the given tree structure (set)
	*
	* @param root root of the tree/set structure
	* @return newick string
	*/
	static string print_tree(set * root, std::function<string(const uint64_t&)> map);



};
