/**
 * @file evaluator.cpp
 * @brief Implementation of evaluation result and context
 */

#include "calc/core/evaluator.h"
#include "calc/utils/error.h"
#include <cmath>
#include <sstream>

namespace calc {

//=============================================================================
// EvaluationResult Implementation
//=============================================================================

EvaluationResult::EvaluationResult(double value)
    : data_(Success{value})
{}

EvaluationResult::EvaluationResult(ErrorCode code, const std::string& message, size_t position)
    : data_(Error{code, message, position})
{}

bool EvaluationResult::isSuccess() const noexcept {
    return std::holds_alternative<Success>(data_);
}

bool EvaluationResult::isError() const noexcept {
    return std::holds_alternative<Error>(data_);
}

double EvaluationResult::getValue() const {
    if (isError()) {
        throw std::runtime_error("Cannot get value from error result");
    }
    return std::get<Success>(data_).value;
}

ErrorCode EvaluationResult::getErrorCode() const {
    if (isSuccess()) {
        throw std::runtime_error("Cannot get error code from successful result");
    }
    return std::get<Error>(data_).code;
}

const std::string& EvaluationResult::getErrorMessage() const {
    if (isSuccess()) {
        throw std::runtime_error("Cannot get error message from successful result");
    }
    return std::get<Error>(data_).message;
}

size_t EvaluationResult::getErrorPosition() const {
    if (isSuccess()) {
        throw std::runtime_error("Cannot get error position from successful result");
    }
    return std::get<Error>(data_).position;
}

std::string EvaluationResult::toString() const {
    if (isSuccess()) {
        std::ostringstream oss;
        oss << std::get<Success>(data_).value;
        return oss.str();
    } else {
        const auto& err = std::get<Error>(data_);
        std::ostringstream oss;
        oss << "[" << errorCodeToString(err.code) << "] " << err.message;
        if (err.position > 0) {
            oss << " (position " << err.position << ")";
        }
        return oss.str();
    }
}

//=============================================================================
// EvaluationContext Implementation
//=============================================================================

EvaluationContext::EvaluationContext(int precision)
    : precision_(precision)
{}

int EvaluationContext::getPrecision() const noexcept {
    return precision_;
}

void EvaluationContext::setPrecision(int precision) {
    if (precision < 0) {
        throw std::invalid_argument("Precision must be non-negative");
    }
    precision_ = precision;
}

bool EvaluationContext::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

void EvaluationContext::addFunction(
    const std::string& name,
    std::function<double(const std::vector<double>&)> callback)
{
    functions_[name] = std::move(callback);
}

EvaluationResult EvaluationContext::callFunction(
    const std::string& name,
    const std::vector<double>& args)
{
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        return EvaluationResult(ErrorCode::INVALID_FUNCTION,
            "Unknown function: " + name);
    }

    try {
        double result = it->second(args);
        return EvaluationResult(result);
    } catch (const CalculatorException& e) {
        // Preserve the original error code from CalculatorException
        return EvaluationResult(e.getErrorCode(), e.what(), e.getPosition());
    } catch (const std::exception& e) {
        return EvaluationResult(ErrorCode::EVALUATION_ERROR,
            "Error calling function '" + name + "': " + e.what());
    }
}

//=============================================================================
// Evaluator Implementation
//=============================================================================

EvaluationResult Evaluator::evaluate(const ASTNode* node) {
    EvaluationContext context;
    return evaluate(node, context);
}

//=============================================================================
// MathFunctions Implementation
//=============================================================================

void MathFunctions::registerBuiltInFunctions(EvaluationContext& context) {
    // Trigonometric functions
    context.addFunction("sin", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("sin requires exactly 1 argument");
        }
        return std::sin(args[0]);
    });

    context.addFunction("cos", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("cos requires exactly 1 argument");
        }
        return std::cos(args[0]);
    });

    context.addFunction("tan", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("tan requires exactly 1 argument");
        }
        return std::tan(args[0]);
    });

    context.addFunction("asin", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("asin requires exactly 1 argument");
        }
        if (args[0] < -1.0 || args[0] > 1.0) {
            throw CalculatorException(ErrorCode::DOMAIN_ERROR, "asin argument must be in [-1, 1]", 0);
        }
        return std::asin(args[0]);
    });

    context.addFunction("acos", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("acos requires exactly 1 argument");
        }
        if (args[0] < -1.0 || args[0] > 1.0) {
            throw CalculatorException(ErrorCode::DOMAIN_ERROR, "acos argument must be in [-1, 1]", 0);
        }
        return std::acos(args[0]);
    });

    context.addFunction("atan", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("atan requires exactly 1 argument");
        }
        return std::atan(args[0]);
    });

    // Hyperbolic functions
    context.addFunction("sinh", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("sinh requires exactly 1 argument");
        }
        return std::sinh(args[0]);
    });

    context.addFunction("cosh", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("cosh requires exactly 1 argument");
        }
        return std::cosh(args[0]);
    });

    context.addFunction("tanh", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("tanh requires exactly 1 argument");
        }
        return std::tanh(args[0]);
    });

    // Logarithmic and exponential functions
    context.addFunction("log", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("log requires exactly 1 argument");
        }
        if (args[0] <= 0.0) {
            throw CalculatorException(ErrorCode::DOMAIN_ERROR, "log argument must be positive", 0);
        }
        return std::log(args[0]);
    });

    context.addFunction("log10", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("log10 requires exactly 1 argument");
        }
        if (args[0] <= 0.0) {
            throw std::domain_error("log10 argument must be positive");
        }
        return std::log10(args[0]);
    });

    context.addFunction("exp", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("exp requires exactly 1 argument");
        }
        return std::exp(args[0]);
    });

    context.addFunction("sqrt", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("sqrt requires exactly 1 argument");
        }
        if (args[0] < 0.0) {
            throw CalculatorException(ErrorCode::DOMAIN_ERROR, "sqrt argument must be non-negative", 0);
        }
        return std::sqrt(args[0]);
    });

    context.addFunction("cbrt", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("cbrt requires exactly 1 argument");
        }
        return std::cbrt(args[0]);
    });

    context.addFunction("pow", [](const std::vector<double>& args) -> double {
        if (args.size() != 2) {
            throw std::invalid_argument("pow requires exactly 2 arguments");
        }
        return std::pow(args[0], args[1]);
    });

    // Rounding and absolute functions
    context.addFunction("abs", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("abs requires exactly 1 argument");
        }
        return std::abs(args[0]);
    });

    context.addFunction("floor", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("floor requires exactly 1 argument");
        }
        return std::floor(args[0]);
    });

    context.addFunction("ceil", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("ceil requires exactly 1 argument");
        }
        return std::ceil(args[0]);
    });

    context.addFunction("round", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("round requires exactly 1 argument");
        }
        return std::round(args[0]);
    });

    context.addFunction("trunc", [](const std::vector<double>& args) -> double {
        if (args.size() != 1) {
            throw std::invalid_argument("trunc requires exactly 1 argument");
        }
        return std::trunc(args[0]);
    });

    // Other functions
    context.addFunction("fmod", [](const std::vector<double>& args) -> double {
        if (args.size() != 2) {
            throw std::invalid_argument("fmod requires exactly 2 arguments");
        }
        if (args[1] == 0.0) {
            throw std::domain_error("fmod divisor cannot be zero");
        }
        return std::fmod(args[0], args[1]);
    });

    context.addFunction("remainder", [](const std::vector<double>& args) -> double {
        if (args.size() != 2) {
            throw std::invalid_argument("remainder requires exactly 2 arguments");
        }
        if (args[1] == 0.0) {
            throw std::domain_error("remainder divisor cannot be zero");
        }
        return std::remainder(args[0], args[1]);
    });

    context.addFunction("max", [](const std::vector<double>& args) -> double {
        if (args.size() < 2) {
            throw std::invalid_argument("max requires at least 2 arguments");
        }
        return *std::max_element(args.begin(), args.end());
    });

    context.addFunction("min", [](const std::vector<double>& args) -> double {
        if (args.size() < 2) {
            throw std::invalid_argument("min requires at least 2 arguments");
        }
        return *std::min_element(args.begin(), args.end());
    });

    context.addFunction("hypot", [](const std::vector<double>& args) -> double {
        if (args.size() != 2) {
            throw std::invalid_argument("hypot requires exactly 2 arguments");
        }
        return std::hypot(args[0], args[1]);
    });

    // Constants (as functions with no arguments)
    context.addFunction("PI", [](const std::vector<double>&) -> double {
        return M_PI;
    });

    context.addFunction("E", [](const std::vector<double>&) -> double {
        return M_E;
    });
}

// Standalone function implementations for direct use
double MathFunctions::sin(double x) { return std::sin(x); }
double MathFunctions::cos(double x) { return std::cos(x); }
double MathFunctions::tan(double x) { return std::tan(x); }
double MathFunctions::asin(double x) { return std::asin(x); }
double MathFunctions::acos(double x) { return std::acos(x); }
double MathFunctions::atan(double x) { return std::atan(x); }
double MathFunctions::sinh(double x) { return std::sinh(x); }
double MathFunctions::cosh(double x) { return std::cosh(x); }
double MathFunctions::tanh(double x) { return std::tanh(x); }
double MathFunctions::log(double x) { return std::log(x); }
double MathFunctions::log10(double x) { return std::log10(x); }
double MathFunctions::exp(double x) { return std::exp(x); }
double MathFunctions::sqrt(double x) { return std::sqrt(x); }
double MathFunctions::cbrt(double x) { return std::cbrt(x); }
double MathFunctions::pow(double x, double y) { return std::pow(x, y); }
double MathFunctions::abs(double x) { return std::abs(x); }
double MathFunctions::floor(double x) { return std::floor(x); }
double MathFunctions::ceil(double x) { return std::ceil(x); }
double MathFunctions::round(double x) { return std::round(x); }
double MathFunctions::trunc(double x) { return std::trunc(x); }
double MathFunctions::fmod(double x, double y) { return std::fmod(x, y); }
double MathFunctions::remainder(double x, double y) { return std::remainder(x, y); }
double MathFunctions::max(double a, double b) { return std::max(a, b); }
double MathFunctions::min(double a, double b) { return std::min(a, b); }
double MathFunctions::hypot(double x, double y) { return std::hypot(x, y); }

} // namespace calc
