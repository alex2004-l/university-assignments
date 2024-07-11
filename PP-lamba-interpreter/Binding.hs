module Binding where

import Lambda

type Context = [(String, Lambda)]

data Line = Eval Lambda 
          | Binding String Lambda deriving (Eq)

instance Show Line where
    show (Eval l) = show l
    show (Binding s l) = s ++ " = " ++ show l

-- 3.1.
lookupMacro :: Context -> String -> Either String Lambda
lookupMacro ctx name =
    case lookup name ctx of
        Just expr -> Right expr
        Nothing -> Left $ "Macro not found: " ++ name

expandMacro :: Context -> Lambda -> Either String Lambda
expandMacro ctx (Macro m) = do
    value <- lookupMacro ctx m
    expandMacro ctx value
expandMacro ctx (Abs x exp) = do
    exp' <- expandMacro ctx exp
    return $ Abs x exp'
expandMacro ctx (App e1 e2) = do
    e1' <- expandMacro ctx e1
    e2' <- expandMacro ctx e2
    return $ App e1' e2'
expandMacro _ other = Right other

simplifyCtx :: Context -> (Lambda -> Lambda) -> Lambda -> Either String [Lambda]
simplifyCtx ctx f expr = case expandMacro ctx expr of
    Left err -> Left err
    Right lambda -> Right $ simplify f lambda

normalCtx :: Context -> Lambda -> Either String [Lambda]
normalCtx ctx = simplifyCtx ctx normalStep

applicativeCtx :: Context -> Lambda -> Either String [Lambda]
applicativeCtx ctx = simplifyCtx ctx applicativeStep
