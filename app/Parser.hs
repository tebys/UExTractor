module Parser where

import           Foreign.C.String
import           Foreign.Marshal.Array (peekArray)
import           Foreign.Ptr

data QueryResult;

foreign import ccall unsafe "do_query" c_do_query :: CString -> CString -> IO (Ptr QueryResult)
foreign import ccall safe "result_size" result_size :: Ptr QueryResult -> Int
foreign import ccall safe "result_values" result_values :: Ptr QueryResult -> Ptr CString
foreign import ccall safe "free_result" free_result :: Ptr QueryResult -> IO ()

doQuery :: String -> String -> IO ([String])
doQuery code query = do
  qr <- withCString code $ \ccode -> withCString query $ \cquery -> c_do_query ccode cquery
  let cant = result_size qr
      vals = result_values qr
  ret <- peekArray cant vals >>= mapM peekCString
  free_result qr
  pure ret
