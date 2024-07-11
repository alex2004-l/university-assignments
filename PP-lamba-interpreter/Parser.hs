module Parser (parseLambda, parseLine) where

import Control.Monad
import Control.Applicative
import Data.Char

import Lambda
import Binding

newtype Parser a = Parser { parse :: String -> Maybe (a, String) }

failParser :: Parser a 
failParser = Parser $ const Nothing

charParser :: Char -> Parser Char
charParser ch = Parser $ \s -> case s of
    [] -> Nothing
    (x : xs) -> if x == ch then Just (x, xs) else Nothing

predicateParser :: (Char -> Bool) -> Parser Char
predicateParser f = Parser $ \s -> case s of
    [] -> Nothing
    (x : xs) -> if f x then Just (x, xs) else Nothing

instance Monad Parser where
    mp >>= f = Parser $ \s -> case parse mp s of
        Nothing -> Nothing
        Just (val, rest) -> parse (f val) rest

    return x = Parser $ \s -> Just (x, s)

instance Applicative Parser where
    af <*> mp = do
        f <- af
        v <- mp
        return $ f v
    pure = return

instance Functor Parser where 
    fmap f mp = do
        x <- mp
        return $ f x

instance Alternative Parser where
    empty = failParser
    p1 <|> p2 = Parser $ \s -> case parse p1 s of
        Nothing -> parse p2 s
        x -> x

plusParser :: Parser a -> Parser [a]
plusParser p = do
    x <- p
    xs <- starParser p
    return (x:xs)

starParser :: Parser a -> Parser [a]
starParser p = plusParser p <|> pure []

varParser :: Parser String
varParser = plusParser (predicateParser isLower)

varLambdaParser :: Parser Lambda
varLambdaParser = Var <$> varParser

isUpperOrDigit :: Char -> Bool
isUpperOrDigit c = isUpper c || isDigit c

macroParser :: Parser String
macroParser = plusParser (predicateParser isUpperOrDigit)

macroLambdaParser :: Parser Lambda
macroLambdaParser = Macro <$> macroParser

whitespaceParser :: Parser String
whitespaceParser = starParser (charParser ' ')

absLambdaParser :: Parser Lambda
absLambdaParser = do
    _ <- charParser '\\'
    whitespaceParser
    var <- varParser
    whitespaceParser
    _ <- charParser '.'
    whitespaceParser
    body <- exprParser
    return $ Abs var body

appLambdaParser :: Parser Lambda
appLambdaParser = do
    _ <- charParser '('
    whitespaceParser
    e1 <- exprParser
    whitespaceParser
    e2 <- exprParser
    whitespaceParser
    _ <- charParser ')'
    return $ App e1 e2

exprParser :: Parser Lambda
exprParser = absLambdaParser <|> varLambdaParser <|> appLambdaParser <|> macroLambdaParser

evalParser :: Parser Line
evalParser = do
    x <- exprParser
    return $ Eval x 

bindingParser :: Parser Line
bindingParser = do
    x <- macroParser
    whitespaceParser
    charParser '='
    whitespaceParser
    y <- exprParser
    return $ Binding x y 

lineParser :: Parser Line
lineParser =  bindingParser <|> evalParser

-- 2.1. / 3.2.
parseLambda :: String -> Lambda
parseLambda input = case parse exprParser input of
    Just (result, "") -> result
    _ -> error "Failed to parse lambda expr"

-- 3.3.
parseLine :: String -> Either String Line
parseLine input = case parse lineParser input of
    Just (result, "") -> Right result
    _ -> Left "Failed to parse line" -- pattern-uri incluse: Just(r, str != "") && Nothing

