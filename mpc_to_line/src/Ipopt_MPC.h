#ifndef IPOPT_MPC_H
#define IPOPT_MPC_H

#include <vector>
#include <IpTNLP.hpp>
#include "IpIpoptApplication.hpp"

/** C++ MPC Implementation NLP for interfacing a problem with IPOPT.
 *  IpoptMPC implements a C++ example of using MPC control strategy
 *  to track a geometrical path.
 *
 *  Problem IpoptMPC looks like this:
 *
 *     MPC problem formation:
 *
 *     Starting point @ each iteration:
 *        x_ini = (...)
 *
 *     Optimal solution @ each iteration:
 *        x_out = (...)
 *
 */

class IpoptMPC : public Ipopt::TNLP 
{
  public:
    /** defualt constructor **/
    IpoptMPC();

    /** default destructor **/
    virtual ~IpoptMPC();

    /**@name Overload from TNLP **/
    //@{
    /** Method to return some info about the nlp **/
    virtual bool get_nlp_info(Ipopt::Index& n, Ipopt::Index& m, Ipopt::Index& nnz_jac_g,
                              Ipopt::Index& nnz_h_lag, TNLP::IndexStyleEnum& index_style);

    /** Method to return the bounds for my problem **/
    virtual bool get_bounds_info(Ipopt::Index n, Ipopt::Number* x_l, Ipopt::Number* x_u,
                                 Ipopt::Index m, Ipopt::Number* g_l, Ipopt::Number* g_u);

    /** Method to return the starting point for the algorithm **/
    virtual bool get_starting_point(Ipopt::Index n, bool init_x, Ipopt::Number* x,
                                    bool init_z, Ipopt::Number* z_L, Ipopt::Number* z_U,
                                    Ipopt::Index m, bool init_lambda,
                                    Ipopt::Number* lambda);
    
    /** Method to return the objective value **/
    virtual bool eval_f(Ipopt::Index n, const Ipopt::Number* x, bool new_x, Ipopt::Number& obj_value);

    /** Method to return the gradient of the objective **/
    virtual bool eval_grad_f(Ipopt::Index n, const Ipopt::Number* x, bool new_x, Ipopt::Number* grad_f);

    /** Method to return the constraint residuals **/
    virtual bool eval_g(Ipopt::Index n, const Ipopt::Number* x, bool new_x, Ipopt::Index m, Ipopt::Number* g);

    /** Method to return:
     *   1) The structure of the jacobian (if "values" is NULL)
     *   2) The values of the jacobian (if "values" is not NULL)
     */
    virtual bool eval_jac_g(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                            Ipopt::Index m, Ipopt::Index nele_jac, Ipopt::Index* iRow, Ipopt::Index *jCol,
                            Ipopt::Number* values);

    /** Method to return:
     *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
     *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
     */
    virtual bool eval_h(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                        Ipopt::Number obj_factor, Ipopt::Index m, const Ipopt::Number* lambda,
                        bool new_lambda, Ipopt::Index nele_hess, Ipopt::Index* iRow,
                        Ipopt::Index* jCol, Ipopt::Number* values);
    //@}

    /** @name Solution Methods */
    //@{
    /** This method is called when the algorithm is complete so the TNLP can store/write the solution */
    virtual void finalize_solution(Ipopt::SolverReturn status,
                                  Ipopt::Index n, const Ipopt::Number* x, const Ipopt::Number* z_L, const Ipopt::Number* z_U,
                                  Ipopt::Index m, const Ipopt::Number* g, const Ipopt::Number* lambda,
                                  Ipopt::Number obj_value,
                                  const Ipopt::IpoptData* ip_data,
                                  Ipopt::IpoptCalculatedQuantities* ip_cq);
    //@}


    // Solve the model given an initial state.
    // Return the next state and actuations as a vector.
    std::vector<double> Solve(const Eigen::VectorXd &x0, 
                              const Eigen::VectorXd &coeffs);
    std::vector<double> Solve(const Eigen::VectorXd& x0);

  private:
    /**@name Methods to block default compiler methods.
     * The compiler automatically generates the following three methods.
     *  Since the default compiler implementation is generally not what
     *  you want (for all but the most simple classes), we usually
     *  put the declarations of these methods in the private section
     *  and never implement them. This prevents the compiler from
     *  implementing an incorrect "default" behavior without us
     *  knowing. (See Scott Meyers book, "Effective C++")
     * 
     */
    //@{
    //  IpoptMPC();
    IpoptMPC(const IpoptMPC&);
    IpoptMPC& operator=(const IpoptMPC&);
    //@}    


    std::vector<std::vector<double>> waypoints_;
    std::vector<double> cl_x_; // center line x
    std::vector<double> cl_y_; // center line y
    std::vector<double> cl_phi_; // center line direction phi

    void readRoadmapFromCSV(const std::string& roadmap_file_name)
    {
        // Parse the roadmap file and add waypoints
        std::istringstream rm_stream(roadmap_file_name);
        std::string line;
        while (std::getline(rm_stream, line))
            parseRoadMapLine(line);
    }

    void parseRoadMapLine(const std::string& line)
    {
        // Parse each line in roadmap file and add waypoint vector
        std::istringstream line_stream(line);
        std::string number;
        std::vector<double> wp;
        while (std::getline(line_stream, number, ','))
            wp.push_back(std::stof(number));
        waypoints_.push_back(wp);
    }

    /** Other variables that assist the MPC problem formulation **/
    std::vector<double> x0_;                      // initial state variables
    int map_sz_;                                  // size of segments of the map
    std::vector<std::vector<double>> waypoints_;  // stored waypoints info from csv
    std::vector<double> cl_x_;                    // center line x
    std::vector<double> cl_y_;                    // center line y
    std::vector<double> cl_phi_;                  // center line direction phi

};

#endif  // IPOPT_MPC_H