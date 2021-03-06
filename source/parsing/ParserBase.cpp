//------------------------------------------------------------------------------
// ParserBase.cpp
// Base class for parsing.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#include "ParserBase.h"

#include "lexing/Preprocessor.h"
#include "util/BumpAllocator.h"

namespace slang {

ParserBase::ParserBase(Preprocessor& preprocessor) :
    window(preprocessor),
    alloc(preprocessor.getAllocator())
{
}

void ParserBase::reduceSkippedTokens(SmallVector<Token>& skipped, SmallVector<Trivia>& trivia) {
    if (skipped.empty())
        return;
    trivia.append(Trivia(TriviaKind::SkippedTokens, skipped.copy(alloc)));
    skipped.clear();
}

SyntaxNode* ParserBase::prependTrivia(SyntaxNode* node, Trivia* trivia) {
    if (trivia->kind != TriviaKind::Unknown && node) {
        Token newToken = prependTrivia(node->getFirstToken(), trivia);
        bool result = node->replaceFirstToken(newToken);
        ASSERT(result);
    }
    return node;
}

Token ParserBase::prependTrivia(Token token, Trivia* trivia) {
    if (trivia->kind != TriviaKind::Unknown && token) {
        SmallVectorSized<Trivia, 8> buffer;
        buffer.append(*trivia);
        buffer.appendRange(token.trivia());

        token = token.withTrivia(alloc, buffer.copy(alloc));
        *trivia = Trivia();
    }
    return token;
}

Token ParserBase::prependTrivia(Token token, SmallVector<Trivia>& trivia) {
    ASSERT(token);
    trivia.appendRange(token.trivia());
    token = token.withTrivia(alloc, trivia.copy(alloc));
    trivia.clear();
    return token;
}

void ParserBase::prependTrivia(SyntaxNode* node, SmallVector<Trivia>& trivia) {
    if (!trivia.empty()) {
        ASSERT(node);
        Token newToken = prependTrivia(node->getFirstToken(), trivia);
        bool result = node->replaceFirstToken(newToken);
        ASSERT(result);
    }
}

SyntaxNode* ParserBase::prependSkippedTokens(SyntaxNode* node, SmallVector<Token>& tokens) {
    if (!tokens.empty()) {
        Trivia t(TriviaKind::SkippedTokens, tokens.copy(alloc));
        prependTrivia(node, &t);
        tokens.clear();
    }
    return node;
}

Token ParserBase::prependSkippedTokens(Token token, SmallVector<Token>& tokens) {
    if (!tokens.empty()) {
        Trivia t(TriviaKind::SkippedTokens, tokens.copy(alloc));
        prependTrivia(token, &t);
        tokens.clear();
    }
    return token;
}

Diagnostics& ParserBase::getDiagnostics() {
    return window.tokenSource.getDiagnostics();
}

Diagnostic& ParserBase::addError(DiagCode code, SourceLocation location) {
    return getDiagnostics().add(code, location);
}

Token ParserBase::peek(int offset) {
    while (window.currentOffset + offset >= window.count)
        window.addNew();
    return window.buffer[window.currentOffset + offset];
}

Token ParserBase::peek() {
    if (!window.currentToken) {
        if (window.currentOffset >= window.count)
            window.addNew();
        window.currentToken = window.buffer[window.currentOffset];
    }
    ASSERT(window.currentToken);
    return window.currentToken;
}

bool ParserBase::peek(TokenKind kind) {
    return peek().kind == kind;
}

Token ParserBase::consume() {
    auto result = peek();
    window.moveToNext();
    return result;
}

Token ParserBase::consumeIf(TokenKind kind) {
    auto result = peek();
    if (result.kind != kind)
        return Token();

    window.moveToNext();
    return result;
}

Token ParserBase::expect(TokenKind kind) {
    // keep this method small so that it gets inlined
    auto result = peek();
    if (result.kind != kind)
        return Token::createExpected(alloc, getDiagnostics(), result, kind, window.lastConsumed);

    window.moveToNext();
    return result;
}

void ParserBase::Window::addNew() {
    if (count >= capacity) {
        // shift tokens to the left if we are too far to the right
        if (currentOffset > (capacity >> 1)) {
            int shift = count - currentOffset;
            if (shift > 0)
                memmove(buffer, buffer + currentOffset, shift * sizeof(Token));

            count -= currentOffset;
            currentOffset = 0;
        }
        else {
            capacity *= 2;
            Token* newBuffer = new Token[capacity];
            memcpy(newBuffer, buffer, count * sizeof(Token));

            delete[] buffer;
            buffer = newBuffer;
        }
    }
    buffer[count] = tokenSource.next();
    count++;
}

void ParserBase::Window::moveToNext() {
    lastConsumed = currentToken;
    currentToken = Token();
    currentOffset++;
}

}