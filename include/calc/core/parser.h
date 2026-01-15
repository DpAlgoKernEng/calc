/**
 * @file parser.h
 * @brief Parser interface base class
 */

#ifndef CALC_CORE_PARSER_H
#define CALC_CORE_PARSER_H

#include "calc/core/ast.h"
#include "calc/core/token.h"
#include <vector>
#include <memory>

namespace calc {

/**
 * @brief Abstract base class for parsers
 *
 * Different parsing algorithms can be implemented as subclasses.
 * Phase 2 implements ShuntingYardParser. Future phases may add
 * RecursiveDescentParser for different use cases.
 */
class Parser {
public:
    virtual ~Parser() = default;

    /**
     * @brief Parse a token stream into an AST
     * @param tokens The vector of tokens to parse
     * @return Unique pointer to the root AST node
     * @throws SyntaxError on parsing errors
     */
    virtual std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) = 0;

    /**
     * @brief Get the name of this parser implementation
     */
    virtual std::string getName() const = 0;
};

} // namespace calc

#endif // CALC_CORE_PARSER_H
