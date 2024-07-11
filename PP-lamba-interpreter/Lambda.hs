module Lambda where

import Data.List (nub, (\\))

data Lambda = Var String
            | App Lambda Lambda
            | Abs String Lambda
            | Macro String

instance Show Lambda where
    show (Var x) = x
    show (App e1 e2) = "(" ++ show e1 ++ " " ++ show e2 ++ ")"
    show (Abs x e) = "λ" ++ x ++ "." ++ show e
    show (Macro x) = x

instance Eq Lambda where
    e1 == e2 = eq e1 e2 ([],[],[])
      where
        eq (Var x) (Var y) (env,xb,yb) = elem (x,y) env || (not $ elem x xb || elem y yb)
        eq (App e1 e2) (App f1 f2) env = eq e1 f1 env && eq e2 f2 env
        eq (Abs x e) (Abs y f) (env,xb,yb) = eq e f ((x,y):env,x:xb,y:yb)
        eq (Macro x) (Macro y) _ = x == y
        eq _ _ _ = False

-- 1.1.
vars :: Lambda -> [String]
vars (Var str) = [str]
vars (App l1 l2) = nub $ (vars l1) ++ (vars l2)
vars (Abs str l) = nub $ [str] ++ (vars l) 

-- 1.2.
freeVars :: Lambda -> [String]
freeVars (Var str) = [str]
freeVars (App l1 l2) = nub $ (freeVars l1) ++ (freeVars l2)
freeVars (Abs str l) = (freeVars l) \\ [str]

-- 1.3.
-- sursa : https://stackoverflow.com/questions/9542313/how-to-generate-a-list-of-all-possible-strings-from-shortest-to-longest
genStr :: [Char] -> Int -> [String]
genStr list 1 = map (:[]) list
genStr list n = concatMap (\head -> map (head ++) (genStr list 1)) (genStr list (n - 1))

genAllStr :: [String]
genAllStr = concatMap (genStr ['a'..'z']) [1..]

firstElem :: [String] -> String
firstElem l = head l

newVar :: [String] -> String
newVar list = firstElem $ genAllStr \\ list

-- 1.4.
isNormalForm :: Lambda -> Bool
isNormalForm (Var x) = True
isNormalForm (Abs x e) = isNormalForm e
isNormalForm (App (Abs x y) e) = False
isNormalForm (App e1 e2) = isNormalForm e1 && isNormalForm e2

-- 1.5.
reduce :: String -> Lambda -> Lambda -> Lambda
reduce x (Var y) e2 = if (x == y) then e2 else (Var y)
reduce x (App a b) m = App (reduce x a m) (reduce x b m)
reduce x (Abs y e) e2 = if (x /= y && (y `elem` (freeVars e2)) == False)
                        then (Abs y (reduce x e e2))
                        else if (x /= y && (y `elem` (freeVars e2)) == True)
                        then (reduce x (Abs z (reduce y e (Var z))) e2)
                        else (Abs y e)
                        where z = newVar $ (vars e ++ vars e2)

-- 1.6.
normalStep :: Lambda -> Lambda
normalStep (Var x) = Var x
normalStep (App (Abs x y) e) = (reduce x y e)
normalStep (App (App x y) e) = App (normalStep (App x y)) e
normalStep (App a e) = App a (normalStep e)
normalStep (Abs x e) = Abs x (normalStep e)

-- 1.7.
applicativeStep :: Lambda -> Lambda
applicativeStep (Var x) = Var x
applicativeStep(Abs x e) = Abs x (applicativeStep e)
applicativeStep(App (App x1 x2) e) = App (applicativeStep (App x1 x2)) e
applicativeStep(App e (App x y)) = App e (applicativeStep (App x y))
applicativeStep(App (Abs x y) e) = reduce x y e

-- 1.8.
simplify :: (Lambda -> Lambda) -> Lambda -> [Lambda]
simplify f e = if (isNormalForm e) then [e] else e : (simplify f (f e))

normal :: Lambda -> [Lambda]
normal = simplify normalStep

applicative :: Lambda -> [Lambda]
applicative = simplify applicativeStep
