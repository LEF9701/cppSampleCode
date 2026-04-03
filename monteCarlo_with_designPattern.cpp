/**
 * Monte Carlo Simulation Framework in C++
 *
 * Design Patterns Used:
 *  - Strategy Pattern   : Pluggable random number generators (RNG)
 *  - Template Method    : Simulation algorithm skeleton in base class
 *  - Factory Pattern    : Creates concrete simulation objects
 *  - RAII / smart ptrs  : Resource management throughout
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <memory>
#include <functional>
#include <string>
#include <iomanip>
#include <cassert>

// ============================================================
// STRATEGY PATTERN — Random Number Generator interface
// ============================================================

class IRng {
public:
    virtual ~IRng() = default;
    virtual double uniform()          = 0;   // [0, 1)
    virtual double standard_normal()  = 0;   // N(0,1)
    virtual std::string name() const  = 0;
};

// --- Concrete Strategy: Mersenne Twister ---
class MersenneTwisterRng : public IRng {
    std::mt19937_64              engine_;
    std::uniform_real_distribution<double> uniform_dist_{0.0, 1.0};
    std::normal_distribution<double>       normal_dist_ {0.0, 1.0};
public:
    explicit MersenneTwisterRng(uint64_t seed = std::random_device{}())
        : engine_(seed) {}

    double uniform()         override { return uniform_dist_(engine_); }
    double standard_normal() override { return normal_dist_ (engine_); }
    std::string name() const override { return "Mersenne Twister (MT19937-64)"; }
};

// --- Concrete Strategy: Linear Congruential (demo / deterministic) ---
class LcgRng : public IRng {
    uint64_t state_;
    static constexpr uint64_t A = 6364136223846793005ULL;
    static constexpr uint64_t C = 1442695040888963407ULL;

    double next_uniform() {
        state_ = A * state_ + C;
        return (state_ >> 11) * (1.0 / (1ULL << 53));
    }

    // Box–Muller transform
    bool   has_spare_ = false;
    double spare_     = 0.0;
public:
    explicit LcgRng(uint64_t seed = 42) : state_(seed) {}

    double uniform() override { return next_uniform(); }

    double standard_normal() override {
        if (has_spare_) { has_spare_ = false; return spare_; }
        double u, v, s;
        do {
            u = next_uniform() * 2.0 - 1.0;
            v = next_uniform() * 2.0 - 1.0;
            s = u * u + v * v;
        } while (s >= 1.0 || s == 0.0);
        double m = std::sqrt(-2.0 * std::log(s) / s);
        spare_     = v * m;
        has_spare_ = true;
        return u * m;
    }

    std::string name() const override { return "Linear Congruential Generator (LCG)"; }
};

// ============================================================
// TEMPLATE METHOD PATTERN — Simulation base class
// ============================================================

struct SimulationResult {
    double   estimate;
    double   std_error;
    long long trials;
    std::string label;
};

class MonteCarloSimulation {
public:
    explicit MonteCarloSimulation(std::shared_ptr<IRng> rng, long long trials)
        : rng_(std::move(rng)), trials_(trials)
    {
        assert(trials > 0);
    }

    virtual ~MonteCarloSimulation() = default;

    // Template Method: fixed algorithm skeleton
    SimulationResult run() {
        setup();                          // hook — override if needed

        double sum   = 0.0;
        double sum2  = 0.0;

        for (long long i = 0; i < trials_; ++i) {
            double sample = simulate_once();   // primitive operation
            sum  += sample;
            sum2 += sample * sample;
        }

        double mean     = sum  / static_cast<double>(trials_);
        double variance = sum2 / static_cast<double>(trials_) - mean * mean;
        double std_err  = std::sqrt(variance / static_cast<double>(trials_));

        teardown();                        // hook — override if needed

        return { mean, std_err, trials_, label() };
    }

protected:
    std::shared_ptr<IRng> rng_;
    long long             trials_;

    virtual void   setup()         {}                    // optional hook
    virtual void   teardown()      {}                    // optional hook
    virtual double simulate_once() = 0;                  // primitive (subclass defines)
    virtual std::string label() const = 0;
};

// ============================================================
// CONCRETE SIMULATIONS
// ============================================================

// --- 1. Pi Estimation via Dart-Board method ---
class PiEstimationSim : public MonteCarloSimulation {
public:
    using MonteCarloSimulation::MonteCarloSimulation;

protected:
    double simulate_once() override {
        double x = rng_->uniform() * 2.0 - 1.0;
        double y = rng_->uniform() * 2.0 - 1.0;
        return (x * x + y * y <= 1.0) ? 4.0 : 0.0;   // inside unit circle → 4
    }

    std::string label() const override { return "Pi Estimation (Dartboard)"; }
};

// --- 2. European Call Option Pricing (Black-Scholes framework) ---
struct OptionParams {
    double S0;    // spot price
    double K;     // strike price
    double r;     // risk-free rate
    double sigma; // volatility
    double T;     // time to maturity (years)
};

class EuropeanCallOptionSim : public MonteCarloSimulation {
    OptionParams params_;
public:
    EuropeanCallOptionSim(std::shared_ptr<IRng> rng,
                          long long              trials,
                          const OptionParams&    params)
        : MonteCarloSimulation(std::move(rng), trials), params_(params) {}

protected:
    double simulate_once() override {
        // Geometric Brownian Motion: S_T = S0 * exp((r - 0.5σ²)T + σ√T * Z)
        double Z   = rng_->standard_normal();
        double S_T = params_.S0 * std::exp(
                         (params_.r - 0.5 * params_.sigma * params_.sigma) * params_.T
                         + params_.sigma * std::sqrt(params_.T) * Z);
        double payoff = std::max(S_T - params_.K, 0.0);
        return payoff * std::exp(-params_.r * params_.T);   // discounted payoff
    }

    std::string label() const override { return "European Call Option Price"; }
};

// --- 3. Numerical Integration  ∫₀¹ f(x) dx ---
class NumericalIntegrationSim : public MonteCarloSimulation {
    std::function<double(double)> f_;
    std::string func_label_;
public:
    NumericalIntegrationSim(std::shared_ptr<IRng>              rng,
                             long long                          trials,
                             std::function<double(double)>      f,
                             std::string                        func_label)
        : MonteCarloSimulation(std::move(rng), trials),
          f_(std::move(f)), func_label_(std::move(func_label)) {}

protected:
    double simulate_once() override {
        return f_(rng_->uniform());
    }

    std::string label() const override {
        return "Integral of " + func_label_ + " over [0,1]";
    }
};

// ============================================================
// FACTORY PATTERN — SimulationFactory
// ============================================================

class SimulationFactory {
public:
    enum class RngType { MersenneTwister, LCG };

    static std::shared_ptr<IRng> make_rng(RngType type, uint64_t seed = 0) {
        switch (type) {
            case RngType::MersenneTwister:
                return seed ? std::make_shared<MersenneTwisterRng>(seed)
                            : std::make_shared<MersenneTwisterRng>();
            case RngType::LCG:
                return std::make_shared<LcgRng>(seed ? seed : 42);
        }
        throw std::invalid_argument("Unknown RNG type");
    }

    static std::unique_ptr<MonteCarloSimulation>
    make_pi(RngType rng_type, long long trials, uint64_t seed = 0) {
        return std::make_unique<PiEstimationSim>(make_rng(rng_type, seed), trials);
    }

    static std::unique_ptr<MonteCarloSimulation>
    make_option(RngType rng_type, long long trials,
                const OptionParams& params, uint64_t seed = 0) {
        return std::make_unique<EuropeanCallOptionSim>(
            make_rng(rng_type, seed), trials, params);
    }

    static std::unique_ptr<MonteCarloSimulation>
    make_integral(RngType rng_type, long long trials,
                  std::function<double(double)> f, std::string label,
                  uint64_t seed = 0) {
        return std::make_unique<NumericalIntegrationSim>(
            make_rng(rng_type, seed), trials, std::move(f), std::move(label));
    }
};

// ============================================================
// Pretty printer
// ============================================================

void print_result(const SimulationResult& r, double true_value = -1.0) {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\n  [" << r.label << "]\n";
    std::cout << "  Trials      : " << r.trials    << "\n";
    std::cout << "  Estimate    : " << r.estimate  << "\n";
    std::cout << "  Std Error   : " << r.std_error << "\n";
    if (true_value >= 0.0) {
        std::cout << "  True Value  : " << true_value << "\n";
        std::cout << "  Abs Error   : " << std::abs(r.estimate - true_value) << "\n";
    }
}

// ============================================================
// MAIN
// ============================================================

int main() {
    using Factory = SimulationFactory;
    using RNG     = SimulationFactory::RngType;

    constexpr long long TRIALS = 5'000'000;

    std::cout << "====================================================\n";
    std::cout << "  Monte Carlo Simulation Framework  (C++17)\n";
    std::cout << "====================================================\n";

    // ---- 1. Pi Estimation — two RNG strategies ----
    std::cout << "\n>> Pi Estimation\n";
    {
        auto sim_mt  = Factory::make_pi(RNG::MersenneTwister, TRIALS, /*seed=*/12345);
        auto sim_lcg = Factory::make_pi(RNG::LCG,             TRIALS, /*seed=*/99);

        print_result(sim_mt->run(),  M_PI);
        print_result(sim_lcg->run(), M_PI);
    }

    // ---- 2. European Call Option ----
    std::cout << "\n>> European Call Option Pricing\n";
    {
        // Analytical Black-Scholes price for comparison
        // S=100, K=100, r=0.05, σ=0.2, T=1 → ~10.4506
        OptionParams params{ .S0=100.0, .K=100.0, .r=0.05, .sigma=0.2, .T=1.0 };
        double bs_price = 10.4506;  // analytical Black-Scholes reference

        auto sim = Factory::make_option(RNG::MersenneTwister, TRIALS, params, 42);
        print_result(sim->run(), bs_price);
    }

    // ---- 3. Numerical Integration ----
    std::cout << "\n>> Numerical Integration\n";
    {
        // ∫₀¹ x² dx = 1/3
        auto sim1 = Factory::make_integral(
            RNG::MersenneTwister, TRIALS,
            [](double x) { return x * x; }, "x²", 7);
        print_result(sim1->run(), 1.0 / 3.0);

        // ∫₀¹ 4√(1−x²) dx = π  (quarter-circle area × 4)
        auto sim2 = Factory::make_integral(
            RNG::MersenneTwister, TRIALS,
            [](double x) { return 4.0 * std::sqrt(1.0 - x * x); }, "4√(1−x²)", 9);
        print_result(sim2->run(), M_PI);
    }

    std::cout << "\n====================================================\n";
    return 0;
}
