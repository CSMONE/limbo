#ifndef LIMBO_OPT_NLOPT_HPP
#define LIMBO_OPT_NLOPT_HPP

#include <Eigen/Core>

#include <vector>

#ifndef USE_NLOPT
#warning No NLOpt
#else
#include <nlopt.hpp>
#endif

namespace limbo {
    namespace opt {
#ifdef USE_NLOPT
        template <typename Params, nlopt::algorithm Algorithm = nlopt::LD_MMA>
#else
        template <typename Params>
#endif
        struct NLOpt {
        public:
            template <typename F>
            Eigen::VectorXd operator()(const F& f) const
            {
#ifdef USE_NLOPT
                nlopt::opt opt(Algorithm, f.param_size());

                opt.set_max_objective(this->nlopt_func<F>, (void*)&f);

                std::vector<double> x(f.init().size());
                Eigen::VectorXd::Map(&x[0], f.init().size()) = f.init();

                opt.set_ftol_rel(1e-8);

                double min;

                opt.optimize(x, min);

                return Eigen::VectorXd::Map(x.data(), x.size());
#else
                return Eigen::VectorXd::Constant(f.param_size(), 0);
#endif
            }
#ifdef USE_NLOPT
        protected:
            template <typename F>
            static double nlopt_func(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
            {
                F* f = (F*)(my_func_data);
                Eigen::VectorXd params = Eigen::VectorXd::Map(x.data(), x.size());
                double v;
                if (!grad.empty()) {
                    Eigen::VectorXd g;
                    auto p = f->utility_and_grad(params);
                    v = std::get<0>(p);
                    g = std::get<1>(p);
                    Eigen::VectorXd::Map(&grad[0], g.size()) = g;
                }
                else {
                    v = f->utility(params);
                }
                return v;
            }
#endif
        };
    }
}

#endif
