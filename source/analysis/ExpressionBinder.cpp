//------------------------------------------------------------------------------
// ExpressionBinder.cpp
// Centralized code for convert expressions into AST trees
// (also includes constant folding).
//
// File is under the MIT license:
//------------------------------------------------------------------------------
#include "ExpressionBinder.h"

#include <algorithm>

#include "SemanticModel.h"
#include "ConstantEvaluator.h"

namespace slang {

ExpressionBinder::ExpressionBinder(SemanticModel& sem, const Scope* scope) :
    sem(sem), alloc(sem.getAllocator()), scope(scope)
{
    ASSERT(scope);
}

ExpressionBinder::ExpressionBinder(SemanticModel& sem, const SubroutineSymbol* subroutine) :
    sem(sem), alloc(sem.getAllocator()), subroutine(subroutine)
{
    scope = subroutine->scope;
    ASSERT(scope);
}

BoundExpression* ExpressionBinder::bindExpression(const ExpressionSyntax* syntax) {
    ASSERT(syntax);

    switch (syntax->kind) {
        case SyntaxKind::NullLiteralExpression:
        case SyntaxKind::StringLiteralExpression:
        case SyntaxKind::TimeLiteralExpression:
        case SyntaxKind::WildcardLiteralExpression:
        case SyntaxKind::OneStepLiteralExpression:
            ASSERT("Not yet implemented");
            break;
        case SyntaxKind::IdentifierName:
        case SyntaxKind::IdentifierSelectName:
        case SyntaxKind::ScopedName:
            return bindName(syntax->as<NameSyntax>(), scope);
        case SyntaxKind::RealLiteralExpression:
        case SyntaxKind::IntegerLiteralExpression:
        case SyntaxKind::UnbasedUnsizedLiteralExpression:
            return bindLiteral(syntax->as<LiteralExpressionSyntax>());
        case SyntaxKind::IntegerVectorExpression:
            return bindLiteral(syntax->as<IntegerVectorExpressionSyntax>());
        case SyntaxKind::ParenthesizedExpression:
            return bindExpression(syntax->as<ParenthesizedExpressionSyntax>()->expression);
        case SyntaxKind::UnaryPlusExpression:
        case SyntaxKind::UnaryMinusExpression:
        case SyntaxKind::UnaryBitwiseNotExpression:
            return bindUnaryArithmeticOperator(syntax->as<PrefixUnaryExpressionSyntax>());
        case SyntaxKind::UnaryBitwiseAndExpression:
        case SyntaxKind::UnaryBitwiseOrExpression:
        case SyntaxKind::UnaryBitwiseXorExpression:
        case SyntaxKind::UnaryBitwiseNandExpression:
        case SyntaxKind::UnaryBitwiseNorExpression:
        case SyntaxKind::UnaryBitwiseXnorExpression:
        case SyntaxKind::UnaryLogicalNotExpression:
            return bindUnaryReductionOperator(syntax->as<PrefixUnaryExpressionSyntax>());
        case SyntaxKind::AddExpression:
        case SyntaxKind::SubtractExpression:
        case SyntaxKind::MultiplyExpression:
        case SyntaxKind::DivideExpression:
        case SyntaxKind::ModExpression:
        case SyntaxKind::BinaryAndExpression:
        case SyntaxKind::BinaryOrExpression:
        case SyntaxKind::BinaryXorExpression:
        case SyntaxKind::BinaryXnorExpression:
            return bindArithmeticOperator(syntax->as<BinaryExpressionSyntax>());
        case SyntaxKind::EqualityExpression:
        case SyntaxKind::InequalityExpression:
        case SyntaxKind::CaseEqualityExpression:
        case SyntaxKind::CaseInequalityExpression:
        case SyntaxKind::GreaterThanEqualExpression:
        case SyntaxKind::GreaterThanExpression:
        case SyntaxKind::LessThanEqualExpression:
        case SyntaxKind::LessThanExpression:
        case SyntaxKind::WildcardEqualityExpression:
        case SyntaxKind::WildcardInequalityExpression:
            return bindComparisonOperator(syntax->as<BinaryExpressionSyntax>());
        case SyntaxKind::LogicalAndExpression:
        case SyntaxKind::LogicalOrExpression:
        case SyntaxKind::LogicalImplicationExpression:
        case SyntaxKind::LogicalEquivalenceExpression:
            return bindRelationalOperator(syntax->as<BinaryExpressionSyntax>());
        case SyntaxKind::LogicalShiftLeftExpression:
        case SyntaxKind::LogicalShiftRightExpression:
        case SyntaxKind::ArithmeticShiftLeftExpression:
        case SyntaxKind::ArithmeticShiftRightExpression:
        case SyntaxKind::PowerExpression:
            return bindShiftOrPowerOperator(syntax->as<BinaryExpressionSyntax>());
        case SyntaxKind::AssignmentExpression:
        case SyntaxKind::AddAssignmentExpression:
        case SyntaxKind::SubtractAssignmentExpression:
        case SyntaxKind::MultiplyAssignmentExpression:
        case SyntaxKind::DivideAssignmentExpression:
        case SyntaxKind::ModAssignmentExpression:
        case SyntaxKind::AndAssignmentExpression:
        case SyntaxKind::OrAssignmentExpression:
        case SyntaxKind::XorAssignmentExpression:
        case SyntaxKind::LogicalLeftShiftAssignmentExpression:
        case SyntaxKind::LogicalRightShiftAssignmentExpression:
        case SyntaxKind::ArithmeticLeftShiftAssignmentExpression:
        case SyntaxKind::ArithmeticRightShiftAssignmentExpression:
            return bindAssignmentOperator(syntax->as<BinaryExpressionSyntax>());
        case SyntaxKind::InvocationExpression:
            return bindSubroutineCall(syntax->as<InvocationExpressionSyntax>());
        case SyntaxKind::ConditionalExpression:
            return bindConditionalExpression(syntax->as<ConditionalExpressionSyntax>());
        case SyntaxKind::ConcatenationExpression:
            return bindConcatenationExpression(syntax->as<ConcatenationExpressionSyntax>());
        case SyntaxKind::MultipleConcatenationExpression:
            return bindMultipleConcatenationExpression(syntax->as<MultipleConcatenationExpressionSyntax>());
        case SyntaxKind::ElementSelectExpression:
            return bindSelectExpression(syntax->as<ElementSelectExpressionSyntax>());

            DEFAULT_UNREACHABLE;
    }
    return nullptr;
}

BoundExpression* ExpressionBinder::bindConstantExpression(const ExpressionSyntax* syntax) {
    return bindAndPropagate(syntax);
}

BoundExpression* ExpressionBinder::bindSelfDeterminedExpression(const ExpressionSyntax* syntax) {
    return bindAndPropagate(syntax);
}

BoundExpression* ExpressionBinder::bindAndPropagate(const ExpressionSyntax* syntax) {
    BoundExpression* expr = bindExpression(syntax);
    propagate(expr, expr->type);
    return expr;
}

BoundExpression* ExpressionBinder::bindAssignmentLikeContext(const ExpressionSyntax* syntax, SourceLocation location, const TypeSymbol* assignmentType) {
    BoundExpression* expr = bindAndPropagate(syntax);
    if (expr->bad())
        return expr;

    const TypeSymbol* type = expr->type;
    if (!assignmentType->isAssignmentCompatible(type)) {
        DiagCode code = assignmentType->isCastCompatible(type) ? DiagCode::NoImplicitConversion : DiagCode::BadAssignment;
        addError(code, location) << syntax->sourceRange();
        expr = badExpr(expr);
    }

    if (!propagateAssignmentLike(expr, assignmentType))
        propagate(expr, expr->type);

    // TODO: truncation
    return expr;
}

BoundExpression* ExpressionBinder::bindLiteral(const LiteralExpressionSyntax* syntax) {
    switch (syntax->kind) {
        case SyntaxKind::IntegerLiteralExpression:
            return alloc.emplace<BoundLiteral>(
                syntax,
                sem.getKnownType(SyntaxKind::IntType),
                std::get<SVInt>(syntax->literal.numericValue())
            );
        case SyntaxKind::RealLiteralExpression:
            return alloc.emplace<BoundLiteral>(
                syntax,
                sem.getKnownType(SyntaxKind::RealType),
                std::get<double>(syntax->literal.numericValue())
            );
        case SyntaxKind::UnbasedUnsizedLiteralExpression: {
            // UnsizedUnbasedLiteralExpressions default to a size of 1 in an undetermined
            // context, but can grow
            logic_t val = std::get<logic_t>(syntax->literal.numericValue());
            return alloc.emplace<BoundLiteral>(
                syntax,
                sem.getIntegralType(1, false, val.isUnknown()),
                SVInt(val));
        }

        DEFAULT_UNREACHABLE;
    }
    return nullptr;
}

BoundExpression* ExpressionBinder::bindLiteral(const IntegerVectorExpressionSyntax* syntax) {
    if (syntax->value.isMissing())
        return badExpr(alloc.emplace<BoundLiteral>(syntax, sem.getErrorType(), nullptr));

    const SVInt& value = std::get<SVInt>(syntax->value.numericValue());
    const TypeSymbol* type = sem.getIntegralType(value.getBitWidth(), value.isSigned(), value.hasUnknown());
    return alloc.emplace<BoundLiteral>(syntax, type, value);
}

BoundExpression* ExpressionBinder::bindName(const NameSyntax* syntax, const Scope* currScope) {
    switch (syntax->kind) {
        case SyntaxKind::IdentifierName:
            return bindSimpleName(syntax->as<IdentifierNameSyntax>(), currScope);
        case SyntaxKind::IdentifierSelectName:
            return bindSelectName(syntax->as<IdentifierSelectNameSyntax>(), currScope);
        case SyntaxKind::ScopedName:
            return bindScopedName(syntax->as<ScopedNameSyntax>(), currScope);
        DEFAULT_UNREACHABLE;
    }
    return nullptr;
}

BoundExpression* ExpressionBinder::bindSimpleName(const IdentifierNameSyntax* syntax, const Scope* currScope) {
    // if we have an invalid name token just give up now; the error has already been reported
    StringRef identifier = syntax->identifier.valueText();
    if (!identifier)
        return badExpr(nullptr);

    const Symbol* symbol = currScope->lookup(identifier);
    if (!symbol) {
        addError(DiagCode::UndeclaredIdentifier, syntax->identifier.location()) << identifier;
        return badExpr(nullptr);
    }

    switch (symbol->kind) {
        case SymbolKind::Parameter:
            return alloc.emplace<BoundParameter>(syntax, symbol->as<ParameterSymbol>());
        case SymbolKind::Variable:
        case SymbolKind::FormalArgument:
            return alloc.emplace<BoundVariable>(syntax, (const VariableSymbol*)symbol);

        DEFAULT_UNREACHABLE;
    }
    return nullptr;
}

BoundExpression* ExpressionBinder::bindSelectName(const IdentifierSelectNameSyntax* syntax, const Scope* currScope) {
    // TODO: once we fully support more complex non-integral types and actual support
    // part selects, we need to be able to handle multiple accesses like
    // foo[2 : 4][3 : 1][7 : 8] where each access depends on the type of foo, not just the type of the preceding
    // expression. For now though, we implement the most simple case:
    // foo[SELECT] where foo is an integral type.

    ASSERT(syntax->selectors.count() == 1);
    // spoof this being just a simple ElementSelectExpression
    return bindSelectExpression(syntax,
        bindName(alloc.emplace<IdentifierNameSyntax>(syntax->identifier), currScope), syntax->selectors[0]->selector);
}

BoundExpression* ExpressionBinder::bindScopedName(const ScopedNameSyntax* syntax, const Scope* currScope) {
    // if we have an invalid name token just give up now; the error has already been reported
    if (syntax->separator.kind == TokenKind::DoubleColon) {
        if (syntax->left->kind == SyntaxKind::UnitScope) { // TODO: check if this is correct
            return bindName(syntax->right->as<NameSyntax>(), sem.getPackages());
        } else if (syntax->left->kind == SyntaxKind::IdentifierName) {
            auto package = syntax->left->as<IdentifierNameSyntax>();
            auto pkgSym = sem.getPackages()->lookup(package->identifier.valueText());
            ASSERT(pkgSym && pkgSym->kind == SymbolKind::Module);
            auto nextScope = pkgSym->as<ModuleSymbol>().scope;
            return bindName(syntax->right->as<NameSyntax>(), nextScope);
        }
        ASSERT(false);
    }
    else if (syntax->separator.kind == TokenKind::Dot) {
        if (syntax->left->kind == SyntaxKind::RootScope) { // TODO: check if this is correct
            return bindName(syntax->right->as<NameSyntax>(), sem.getPackages());
        } else if (syntax->left->kind == SyntaxKind::IdentifierName ||
                   syntax->left->kind == SyntaxKind::IdentifierSelectName) { // TODO: check index for demimensions/bounds
            auto parent = syntax->left->as<IdentifierNameSyntax>();
            auto parentSym = currScope->lookup(parent->identifier.valueText());
            ASSERT(parentSym);
            const Scope* nextScope = nullptr;
            if (parentSym->kind == SymbolKind::Instance)
                nextScope = parentSym->as<InstanceSymbol>().module->scope;
            if (parentSym->kind == SymbolKind::Module)
                nextScope = parentSym->as<ModuleSymbol>().scope;
            ASSERT(nextScope);
            return bindName(syntax->right->as<NameSyntax>(), nextScope);
        }
        ASSERT(false);
    }
    return nullptr;
}

BoundExpression* ExpressionBinder::bindUnaryArithmeticOperator(const PrefixUnaryExpressionSyntax* syntax) {
    // Supported for both integral and real types. Can be overloaded for others.
    BoundExpression* operand = bindAndPropagate(syntax->operand);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &operand))
        return badExpr(alloc.emplace<BoundUnaryExpression>(syntax, sem.getErrorType(), operand));

    return alloc.emplace<BoundUnaryExpression>(syntax, operand->type, operand);
}

BoundExpression* ExpressionBinder::bindUnaryReductionOperator(const PrefixUnaryExpressionSyntax* syntax) {
    // Result type is always a single bit. Supported on integral types.
    BoundExpression* operand = bindAndPropagate(syntax->operand);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &operand))
        return badExpr(alloc.emplace<BoundUnaryExpression>(syntax, sem.getErrorType(), operand));

    return alloc.emplace<BoundUnaryExpression>(syntax, sem.getKnownType(SyntaxKind::LogicType), operand);
}

BoundExpression* ExpressionBinder::bindArithmeticOperator(const BinaryExpressionSyntax* syntax) {
    BoundExpression* lhs = bindAndPropagate(syntax->left);
    BoundExpression* rhs = bindAndPropagate(syntax->right);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &lhs, &rhs))
        return badExpr(alloc.emplace<BoundBinaryExpression>(syntax, sem.getErrorType(), lhs, rhs));

    // Get the result type; force the type to be four-state if it's a division, which can make a 4-state output
    // out of 2-state inputs
    const TypeSymbol* type = binaryOperatorResultType(lhs->type, rhs->type, syntax->kind == SyntaxKind::DivideExpression);
    return alloc.emplace<BoundBinaryExpression>(syntax, type, lhs, rhs);
}

BoundExpression* ExpressionBinder::bindComparisonOperator(const BinaryExpressionSyntax* syntax) {
    BoundExpression* lhs = bindAndPropagate(syntax->left);
    BoundExpression* rhs = bindAndPropagate(syntax->right);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &lhs, &rhs))
        return badExpr(alloc.emplace<BoundBinaryExpression>(syntax, sem.getErrorType(), lhs, rhs));

    // result type is always a single bit
    return alloc.emplace<BoundBinaryExpression>(syntax, sem.getKnownType(SyntaxKind::LogicType), lhs, rhs);
}

BoundExpression* ExpressionBinder::bindRelationalOperator(const BinaryExpressionSyntax* syntax) {
    BoundExpression* lhs = bindAndPropagate(syntax->left);
    BoundExpression* rhs = bindAndPropagate(syntax->right);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &lhs, &rhs))
        return badExpr(alloc.emplace<BoundBinaryExpression>(syntax, sem.getErrorType(), lhs, rhs));

    // operands are sized to max(l,r) and the result of the operation is always 1 bit
    // no propagations from above have an actual have an effect on the subexpressions
    // This logic is similar to that of assignment operators, except for the
    // reciprocality
    if (!propagateAssignmentLike(rhs, lhs->type)) {
        propagateAssignmentLike(lhs, rhs->type);
    }

    // result type is always a single bit
    return alloc.emplace<BoundBinaryExpression>(syntax, sem.getKnownType(SyntaxKind::LogicType), lhs, rhs);
}

BoundExpression* ExpressionBinder::bindShiftOrPowerOperator(const BinaryExpressionSyntax* syntax) {
    // The shift and power operators are handled together here because in both cases the second
    // operand is evaluated in a self determined context.
    BoundExpression* lhs = bindAndPropagate(syntax->left);
    BoundExpression* rhs = bindAndPropagate(syntax->right);
    if (!checkOperatorApplicability(syntax->kind, syntax->operatorToken.location(), &lhs, &rhs))
        return badExpr(alloc.emplace<BoundBinaryExpression>(syntax, sem.getErrorType(), lhs, rhs));

    // Power operator can result in division by zero 'x
    const TypeSymbol* type = binaryOperatorResultType(lhs->type, rhs->type, syntax->kind == SyntaxKind::PowerExpression);

    return alloc.emplace<BoundBinaryExpression>(syntax, type, lhs, rhs);
}

BoundExpression* ExpressionBinder::bindAssignmentOperator(const BinaryExpressionSyntax* syntax) {
    BoundExpression* lhs = bindAndPropagate(syntax->left);
    BoundExpression* rhs = bindAndPropagate(syntax->right);

    // Basic assignment (=) is always applicable, but operators like += are applicable iff
    // the associated binary operator is applicable
    SyntaxKind binopKind = SyntaxKind::Unknown;
    switch (syntax->kind) {
        case SyntaxKind::AssignmentExpression: binopKind = SyntaxKind::Unknown; break;
        case SyntaxKind::AddAssignmentExpression: binopKind = SyntaxKind::AddExpression; break;
        case SyntaxKind::SubtractAssignmentExpression: binopKind = SyntaxKind::SubtractExpression; break;
        case SyntaxKind::MultiplyAssignmentExpression: binopKind = SyntaxKind::MultiplyExpression; break;
        case SyntaxKind::DivideAssignmentExpression: binopKind = SyntaxKind::DivideExpression; break;
        case SyntaxKind::ModAssignmentExpression: binopKind = SyntaxKind::ModExpression; break;
        case SyntaxKind::AndAssignmentExpression: binopKind = SyntaxKind::BinaryAndExpression; break;
        case SyntaxKind::OrAssignmentExpression: binopKind = SyntaxKind::BinaryOrExpression; break;
        case SyntaxKind::XorAssignmentExpression: binopKind = SyntaxKind::BinaryXorExpression; break;
        case SyntaxKind::LogicalLeftShiftAssignmentExpression: binopKind = SyntaxKind::LogicalShiftLeftExpression; break;
        case SyntaxKind::LogicalRightShiftAssignmentExpression: binopKind = SyntaxKind::LogicalShiftRightExpression; break;
        case SyntaxKind::ArithmeticLeftShiftAssignmentExpression: binopKind = SyntaxKind::ArithmeticShiftLeftExpression; break;
        case SyntaxKind::ArithmeticRightShiftAssignmentExpression: binopKind = SyntaxKind::ArithmeticShiftRightExpression; break;
        DEFAULT_UNREACHABLE;
    }
    // TODO: the LHS has to be assignable (i.e not a general expression)
    if (!checkOperatorApplicability(binopKind, syntax->operatorToken.location(), &lhs, &rhs))
        return badExpr(alloc.emplace<BoundBinaryExpression>(syntax, sem.getErrorType(), lhs, rhs));

    // The operands of an assignment are themselves self determined,
    // but we must increase the size of the RHS to the size of the LHS if it is larger, and then
    // propagate that information down
    propagateAssignmentLike(rhs, lhs->type);

    // result type is always the type of the left hand side
    return alloc.emplace<BoundAssignmentExpression>(syntax, lhs->type, lhs, rhs);
}

BoundExpression* ExpressionBinder::bindSubroutineCall(const InvocationExpressionSyntax* syntax) {
    // TODO: check for something other than a simple name on the LHS
    auto name = syntax->left->getFirstToken();
    const Symbol* symbol = scope->lookup(name.valueText());
    ASSERT(symbol && symbol->kind == SymbolKind::Subroutine);

    auto actualArgs = syntax->arguments->parameters;
    const SubroutineSymbol& subroutine = symbol->as<SubroutineSymbol>();

    // TODO: handle too few args as well, which requires looking at default values
    if (subroutine.arguments.count() < actualArgs.count()) {
        auto& diag = addError(DiagCode::TooManyArguments, name.location());
        diag << syntax->left->sourceRange();
        diag << subroutine.arguments.count();
        diag << actualArgs.count();
        return badExpr(nullptr);
    }

    // TODO: handle named arguments in addition to ordered
    SmallVectorSized<const BoundExpression*, 8> buffer;
    for (uint32_t i = 0; i < actualArgs.count(); i++) {
        auto arg = (const OrderedArgumentSyntax*)actualArgs[i];
        buffer.append(bindAssignmentLikeContext(arg->expr, arg->sourceRange().start(), subroutine.arguments[i]->type));
    }

    return alloc.emplace<BoundCallExpression>(syntax, &subroutine, buffer.copy(alloc));
}

BoundExpression* ExpressionBinder::bindConditionalExpression(const ConditionalExpressionSyntax* syntax) {
    // TODO: handle the pattern matching conditional predicate case, rather than just assuming that it's a simple
    // expression
    ASSERT(syntax->predicate->conditions.count() == 1);
    BoundExpression* pred = bindAndPropagate(syntax->predicate->conditions[0]->expr);
    BoundExpression* left = bindAndPropagate(syntax->left);
    BoundExpression* right = bindAndPropagate(syntax->right);

    // TODO: handle non-integral and non-real types properly
    // force four-state return type for ambiguous condition case
    const TypeSymbol* type = binaryOperatorResultType(left->type, right->type, true);
    return alloc.emplace<BoundTernaryExpression>(syntax, type, pred, left, right);
}

BoundExpression* ExpressionBinder::bindConcatenationExpression(const ConcatenationExpressionSyntax* syntax) {
    SmallVectorSized<const BoundExpression*, 8> buffer;
    size_t totalWidth = 0;
    for (auto argSyntax : syntax->expressions) {
        const BoundExpression *arg = bindAndPropagate(argSyntax);
        buffer.append(arg);
        const TypeSymbol* type = arg->type;
        if (type->kind != SymbolKind::IntegralType) {
            return badExpr(alloc.emplace<BoundNaryExpression>(syntax, sem.getErrorType(), nullptr));
        }
        totalWidth += type->width();
    }

    return alloc.emplace<BoundNaryExpression>(syntax, sem.getIntegralType(totalWidth, false), buffer.copy(alloc));
}

BoundExpression* ExpressionBinder::bindMultipleConcatenationExpression(const MultipleConcatenationExpressionSyntax* syntax) {
    BoundExpression* left  = bindAndPropagate(syntax->expression);
    BoundExpression* right = bindAndPropagate(syntax->concatenation);
    // TODO: check applicability
    // TODO: left must be compile-time evaluatable, and it must be known in order to
    // compute the type of a multiple concatenation. Have a nice error when this isn't the case?
    // TODO: in cases like these, should we bother storing the bound expression? should we at least cache the result
    // so we don't have to compute it again elsewhere?
    ConstantEvaluator evaluator;
    uint16_t replicationTimes = evaluator.evaluateExpr(left).integer().getAssertUInt16();
    return alloc.emplace<BoundBinaryExpression>(syntax, sem.getIntegralType(right->type->width() * replicationTimes, false), left, right);
}

BoundExpression* ExpressionBinder::bindSelectExpression(const ElementSelectExpressionSyntax* syntax) {
    BoundExpression* expr = bindAndPropagate(syntax->left);
    return bindSelectExpression(syntax, expr, syntax->select->selector);
}

BoundExpression* ExpressionBinder::bindSelectExpression(const ExpressionSyntax* syntax, BoundExpression* expr, const SelectorSyntax* selector) {

    // if (down), the indices are declares going down, [15:0], so
    // msb > lsb
    bool down = expr->type->as<IntegralTypeSymbol>().lowerBounds[0] >= 0;
    BoundExpression* left;
    BoundExpression* right;
    int width = 0;

    ConstantEvaluator evaluator;
    // TODO: errors if things that should be constant expressions aren't actually constant expressions
    SyntaxKind kind = selector->kind;
    switch (kind) {
        case SyntaxKind::BitSelect:
            left = bindAndPropagate(((BitSelectSyntax*)selector)->expr);
            right = left;
            width = 1;
            break;
        case SyntaxKind::SimpleRangeSelect:
            left = bindAndPropagate(((RangeSelectSyntax*)selector)->left); // msb
            right = bindAndPropagate(((RangeSelectSyntax*)selector)->right); // lsb
            width = (down ? 1 : -1) * (evaluator.evaluateExpr(left).integer().getAssertInt64() -
                    evaluator.evaluateExpr(right).integer().getAssertInt64());
            break;
        case SyntaxKind::AscendingRangeSelect:
        case SyntaxKind::DescendingRangeSelect:
            left = bindAndPropagate(((RangeSelectSyntax*)selector)->left); // msb/lsb
            right = bindAndPropagate(((RangeSelectSyntax*)selector)->right); // width
            width = evaluator.evaluateExpr(right).integer().getAssertInt64();
            break;

        DEFAULT_UNREACHABLE;
    }
    return alloc.emplace<BoundSelectExpression>(syntax, sem.getIntegralType(width, expr->type->isSigned(), expr->type->isFourState()), kind, expr, left, right);
}

bool ExpressionBinder::checkOperatorApplicability(SyntaxKind op, SourceLocation location, BoundExpression** operand) {
    if ((*operand)->bad())
        return false;

    const TypeSymbol* type = (*operand)->type;
    bool good;
    switch (op) {
        case SyntaxKind::UnaryPlusExpression:
        case SyntaxKind::UnaryMinusExpression:
        case SyntaxKind::UnaryLogicalNotExpression:
            good = type->kind == SymbolKind::IntegralType || type->kind == SymbolKind::RealType;
            break;
        default:
            good = type->kind == SymbolKind::IntegralType;
            break;
    }
    if (good)
        return true;

    auto& diag = addError(DiagCode::BadUnaryExpression, location);
    diag << type->toString();
    diag << (*operand)->syntax->sourceRange();
    return false;
}

bool ExpressionBinder::checkOperatorApplicability(SyntaxKind op, SourceLocation location, BoundExpression** lhs, BoundExpression** rhs) {
    if ((*lhs)->bad() || (*rhs)->bad())
        return false;

    const TypeSymbol* lt = (*lhs)->type;
    const TypeSymbol* rt = (*rhs)->type;
    bool good;
    switch (op) {
        case SyntaxKind::AddExpression:
        case SyntaxKind::SubtractExpression:
        case SyntaxKind::MultiplyExpression:
        case SyntaxKind::DivideExpression:
        case SyntaxKind::PowerExpression:
        case SyntaxKind::LogicalAndExpression:
        case SyntaxKind::LogicalOrExpression:
        case SyntaxKind::LogicalImplicationExpression:
        case SyntaxKind::LogicalEquivalenceExpression:
        case SyntaxKind::LessThanEqualExpression:
        case SyntaxKind::LessThanExpression:
        case SyntaxKind::GreaterThanEqualExpression:
        case SyntaxKind::GreaterThanExpression:
        case SyntaxKind::EqualityExpression:
        case SyntaxKind::InequalityExpression:
        case SyntaxKind::WildcardEqualityExpression:
        case SyntaxKind::WildcardInequalityExpression:
        case SyntaxKind::CaseEqualityExpression:
        case SyntaxKind::CaseInequalityExpression:
            good = (lt->kind == SymbolKind::IntegralType || lt->kind == SymbolKind::RealType) &&
                   (rt->kind == SymbolKind::IntegralType || rt->kind == SymbolKind::RealType);
            break;
        default:
            good = lt->kind == SymbolKind::IntegralType && rt->kind == SymbolKind::IntegralType;
            break;
    }
    if (good)
        return true;

    auto& diag = addError(DiagCode::BadBinaryExpression, location);
    diag << lt->toString() << rt->toString();
    diag << (*lhs)->syntax->sourceRange();
    diag << (*rhs)->syntax->sourceRange();
    return false;
}

void ExpressionBinder::propagate(BoundExpression* expression, const TypeSymbol* type) {
    ASSERT(expression);
    if (expression->bad())
        return;

    // SystemVerilog rules for width propagation are subtle and very specific
    // to each individual operator type. They also mainly only apply to
    // expressions of integral type (which will be the majority in most designs).

    // If a type of real is propagated to an expression of a non-real type, the type of the
    // direct sub-expression is changed, but it is not propagated any further down
    bool doNotPropogateRealDownToNonReal = type->isReal() && !expression->type->isReal();
    switch (expression->syntax->kind) {
        case SyntaxKind::NullLiteralExpression:
        case SyntaxKind::StringLiteralExpression:
        case SyntaxKind::TimeLiteralExpression:
        case SyntaxKind::WildcardLiteralExpression:
        case SyntaxKind::OneStepLiteralExpression:
        case SyntaxKind::UnbasedUnsizedLiteralExpression:
        case SyntaxKind::IdentifierName:
        case SyntaxKind::RealLiteralExpression:
        case SyntaxKind::IntegerLiteralExpression:
        case SyntaxKind::IntegerVectorExpression:
        case SyntaxKind::InvocationExpression:
            expression->type = type;
            break;
        case SyntaxKind::UnaryPlusExpression:
        case SyntaxKind::UnaryMinusExpression:
        case SyntaxKind::UnaryBitwiseNotExpression:
            expression->type = type;
            if (!doNotPropogateRealDownToNonReal)
                propagate(((BoundUnaryExpression*)expression)->operand, type);
            break;
        case SyntaxKind::UnaryBitwiseAndExpression:
        case SyntaxKind::UnaryBitwiseOrExpression:
        case SyntaxKind::UnaryBitwiseXorExpression:
        case SyntaxKind::UnaryBitwiseNandExpression:
        case SyntaxKind::UnaryBitwiseNorExpression:
        case SyntaxKind::UnaryBitwiseXnorExpression:
        case SyntaxKind::UnaryLogicalNotExpression:
            // Type is already set (always 1 bit) and operand is self determined
            break;
        case SyntaxKind::AddExpression:
        case SyntaxKind::SubtractExpression:
        case SyntaxKind::MultiplyExpression:
        case SyntaxKind::DivideExpression:
        case SyntaxKind::ModExpression:
        case SyntaxKind::BinaryAndExpression:
        case SyntaxKind::BinaryOrExpression:
        case SyntaxKind::BinaryXorExpression:
        case SyntaxKind::BinaryXnorExpression:
            expression->type = type;
            if (!doNotPropogateRealDownToNonReal) {
                propagate(((BoundBinaryExpression*)expression)->left, type);
                propagate(((BoundBinaryExpression*)expression)->right, type);
            }
            break;
        case SyntaxKind::EqualityExpression:
        case SyntaxKind::InequalityExpression:
        case SyntaxKind::CaseEqualityExpression:
        case SyntaxKind::CaseInequalityExpression:
        case SyntaxKind::GreaterThanEqualExpression:
        case SyntaxKind::GreaterThanExpression:
        case SyntaxKind::LessThanEqualExpression:
        case SyntaxKind::LessThanExpression:
        case SyntaxKind::WildcardEqualityExpression:
        case SyntaxKind::WildcardInequalityExpression:
            // Relational expressions are essentially self-detetermined, the logic
            // for how the left and right operands effect eachother is handled
            // at bind time
            break;
        case SyntaxKind::LogicalAndExpression:
        case SyntaxKind::LogicalOrExpression:
        case SyntaxKind::LogicalImplicationExpression:
        case SyntaxKind::LogicalEquivalenceExpression:
            // Type is already set (always 1 bit) and operands are self determined
            break;
        case SyntaxKind::LogicalShiftLeftExpression:
        case SyntaxKind::LogicalShiftRightExpression:
        case SyntaxKind::ArithmeticShiftLeftExpression:
        case SyntaxKind::ArithmeticShiftRightExpression:
        case SyntaxKind::PowerExpression:
            // Only the left hand side gets propagated; the rhs is self determined
            expression->type = type;
            if (!doNotPropogateRealDownToNonReal)
                propagate(((BoundBinaryExpression*)expression)->left, type);
            break;
        case SyntaxKind::AssignmentExpression:
        case SyntaxKind::AddAssignmentExpression:
        case SyntaxKind::SubtractAssignmentExpression:
        case SyntaxKind::MultiplyAssignmentExpression:
        case SyntaxKind::DivideAssignmentExpression:
        case SyntaxKind::ModAssignmentExpression:
        case SyntaxKind::AndAssignmentExpression:
        case SyntaxKind::OrAssignmentExpression:
        case SyntaxKind::XorAssignmentExpression:
        case SyntaxKind::LogicalLeftShiftAssignmentExpression:
        case SyntaxKind::LogicalRightShiftAssignmentExpression:
        case SyntaxKind::ArithmeticLeftShiftAssignmentExpression:
        case SyntaxKind::ArithmeticRightShiftAssignmentExpression:
            // Essentially self determined, logic handled at bind time
            break;
        case SyntaxKind::ConditionalExpression:
            // predicate is self determined
            expression->type = type;
            if (!doNotPropogateRealDownToNonReal) {
                propagate(((BoundTernaryExpression*)expression)->left, type);
                propagate(((BoundTernaryExpression*)expression)->right, type);
            }
            break;
        case SyntaxKind::ConcatenationExpression:
        case SyntaxKind::MultipleConcatenationExpression:
        case SyntaxKind::ElementSelectExpression:
        case SyntaxKind::IdentifierSelectName:
            // all operands are self determined
            expression->type = type;
            break;

        DEFAULT_UNREACHABLE;
    }
}

BoundStatement* ExpressionBinder::bindStatement(const StatementSyntax* syntax) {
    ASSERT(syntax);
    switch (syntax->kind) {
        case SyntaxKind::ReturnStatement: return bindReturnStatement((const ReturnStatementSyntax*)syntax);
        case SyntaxKind::ConditionalStatement: return bindConditionalStatement((const ConditionalStatementSyntax *)syntax);
        case SyntaxKind::ForLoopStatement: return bindForLoopStatement((const ForLoopStatementSyntax *)syntax);
        case SyntaxKind::ExpressionStatement: return bindExpressionStatement((const ExpressionStatementSyntax *)syntax);

        DEFAULT_UNREACHABLE;
    }
    return nullptr;
}

BoundStatementList* ExpressionBinder::bindStatementList(const SyntaxList<SyntaxNode>& items) {
    SmallVectorSized<const BoundStatement*, 8> buffer;
    for (const auto& item : items) {
        if (item->kind == SyntaxKind::DataDeclaration)
            bindVariableDecl((const DataDeclarationSyntax*)item, buffer);
        else if (isStatement(item->kind))
            buffer.append(bindStatement((const StatementSyntax*)item));
    }
    return alloc.emplace<BoundStatementList>(buffer.copy(alloc));
}

BoundStatement* ExpressionBinder::bindReturnStatement(const ReturnStatementSyntax* syntax) {
    auto location = syntax->returnKeyword.location();
    if (!subroutine) {
        addError(DiagCode::ReturnNotInSubroutine, location);
        return badStmt(nullptr);
    }

    auto expr = bindAssignmentLikeContext(syntax->returnValue, location, subroutine->returnType);
    return alloc.emplace<BoundReturnStatement>(syntax, expr);
}

BoundStatement* ExpressionBinder::bindConditionalStatement(const ConditionalStatementSyntax *syntax) {
    ASSERT(syntax);
    ASSERT(syntax->predicate);
    ASSERT(syntax->predicate->conditions.count() == 1,
           " The &&& operator in if condition is not yet supported");
    ASSERT(syntax->predicate->conditions[0]->matchesClause == nullptr,
           " Pattern-matching is not yet supported");
    auto cond = bindExpression(syntax->predicate->conditions[0]->expr);
    auto ifTrue = bindStatement(syntax->statement);
    const BoundStatement* ifFalse = nullptr;
    if (syntax->elseClause) {
        ifFalse = bindStatement((const StatementSyntax *)syntax->elseClause->clause);
    }
    return alloc.emplace<BoundConditionalStatement>(syntax, cond, ifTrue, ifFalse);
}

BoundStatement* ExpressionBinder::bindForLoopStatement(const ForLoopStatementSyntax *syntax) {
    SmallVectorSized<const Symbol*, 2> initializers;
    SmallVectorSized<const BoundVariableDecl*, 2> boundVars;
    Scope *forScope = alloc.emplace<Scope>(scope);

    for (auto initializer : syntax->initializers) {
        auto forVarDecl = (const ForVariableDeclarationSyntax *)initializer;
        const TypeSymbol *type = sem.makeTypeSymbol(forVarDecl->type, forScope);
        sem.handleVariableDeclarator(forVarDecl->declarator, initializers, forScope, {}, type);
    }
    ArrayRef<const Symbol*> initializersRef = initializers.copy(alloc);
    for (auto initializerSym : initializersRef) {
        boundVars.append(alloc.emplace<BoundVariableDecl>((const VariableSymbol*)initializerSym));
    }
    ExpressionBinder binder(sem, forScope);
    auto stopExpr = binder.bindExpression(syntax->stopExpr);
    SmallVectorSized<const BoundExpression*, 2> steps;
    for (auto step : syntax->steps) {
        steps.append(binder.bindExpression(step));
    }
    auto statement = binder.bindStatement(syntax->statement);

    return alloc.emplace<BoundForLoopStatement>(syntax, boundVars.copy(alloc), stopExpr, steps.copy(alloc), statement);
}

void ExpressionBinder::bindVariableDecl(const DataDeclarationSyntax* syntax, SmallVector<const BoundStatement*>& results) {
    // TODO: figure out const-ness of the scope here; shouldn't const cast obviously
    SmallVectorSized<const Symbol*, 8> buffer;
    sem.makeVariables(syntax, buffer, const_cast<Scope*>(scope));

    for (auto symbol : buffer)
        results.append(alloc.emplace<BoundVariableDecl>((const VariableSymbol*)symbol));
}

BoundStatement* ExpressionBinder::bindExpressionStatement(const ExpressionStatementSyntax *syntax) {
    auto expr = bindExpression(syntax->expr);
    return alloc.emplace<BoundExpressionStatement>(syntax, expr);
}

bool ExpressionBinder::propagateAssignmentLike(BoundExpression* rhs, const TypeSymbol* lhsType) {

    // Integral case
    if (lhsType->width() > rhs->type->width()) {
        if (!lhsType->isReal() && !rhs->type->isReal()) {
            rhs->type = sem.getIntegralType(lhsType->width(), rhs->type->isSigned(), rhs->type->isFourState());
        } else {
            if (lhsType->width() > 32) rhs->type = sem.getKnownType(SyntaxKind::RealType);
            else rhs->type = sem.getKnownType(SyntaxKind::ShortRealType);
        }
        propagate(rhs, rhs->type);
        return true;
    }

    return false;
}

const TypeSymbol* ExpressionBinder::binaryOperatorResultType(const TypeSymbol* lhsType, const TypeSymbol* rhsType, bool forceFourState) {
    int width = std::max(lhsType->width(), rhsType->width());
    bool isSigned = lhsType->isSigned() && rhsType->isSigned();
    bool fourState = forceFourState || lhsType->isFourState() || rhsType->isFourState();
    if (lhsType->isReal() || rhsType->isReal()) {
        // spec says that RealTime and RealType are interchangeable, so we will just use RealType for
        // intermediate symbols
        // TODO: The spec is unclear for binary operators what to do if the operands are a shortreal and a larger
        // integral type. For the conditional operator it is clear that this case should lead to a shortreal, and
        // it isn't explicitly mentioned for other binary operators
        if (width >= 64) return sem.getKnownType(SyntaxKind::RealType);
        else return sem.getKnownType(SyntaxKind::ShortRealType);
    } else {
        return sem.getIntegralType(width, isSigned, fourState);
    }
}

BadBoundExpression* ExpressionBinder::badExpr(BoundExpression* expr) {
    return alloc.emplace<BadBoundExpression>(expr);
}

BadBoundStatement* ExpressionBinder::badStmt(BoundStatement* stmt) {
    return alloc.emplace<BadBoundStatement>(stmt);
}

Diagnostic& ExpressionBinder::addError(DiagCode code, SourceLocation location) {
    return sem.getDiagnostics().add(code, location);
}

}
