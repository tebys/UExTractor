module Main where

import           Parser

main :: IO ()
main = do
  code <- readFile "code.c"
  res <- doQuery code "(function_declarator declarator:(_) @name)"
  print res
