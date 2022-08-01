#!/usr/bin/env runghc

module Main where

import Text.Read ( readMaybe )
import Control.Concurrent ( threadDelay )
import Control.Monad ( void, when )
import System.Environment ( getArgs )
import System.Exit ( exitSuccess, exitFailure )
import System.IO ( putStrLn )

import System.Posix.User                        -- from package unix

import System.Posix.Daemonize ( daemonize )     -- from package hdaemonize

import DBus.Notify                              -- from package fdo-notify
    ( connectSession,
      notify,
      Body( Text ),
      Hint( Urgency ),
      Icon( File ),
      Note( Note, expiry, hints, actions, body, summary, appImage, appName ),
      Timeout( Dependent ),
      UrgencyLevel( Critical ) )


notifAppName, notifHeading, notifContent, notifImage :: String
notifAppName = "Teaman"
notifHeading = "Teaman"
notifContent = "Tea!"
notifImage   = {- HOME -} "/.user-utils/tea/icon.png"


msToUs :: Num a => a -> a
msToUs = (*) 1000


theProgram :: Int -> IO ()
theProgram mins = do
    -- wait given number of minutes
    threadDelay $ msToUs 1000 * 60 * mins

    -- get home directory
    home <- homeDirectory <$> (getRealUserID >>= getUserEntryForID)

    -- display system notification
    client <- connectSession
    let note = Note { appName = notifAppName
                    , appImage = (Just . File) (home ++ notifImage)
                    , summary = notifHeading
                    , body = (Just . Text) notifContent
                    , actions = []
                    , hints = [
                        Urgency Critical
                    ]
                    , expiry = Dependent }
    void $ notify client note


main :: IO ()
main = do
    args <- getArgs

    let len = length args
    when (len < 1)
        (putStrLn ("Invalid number of arguments: " ++ show len) >> exitFailure)

    let param = readMaybe $ args !! 0 :: Maybe Int
    case param of
        Nothing -> putStrLn "Invalid number" >> exitFailure
        Just mins -> daemonize $ theProgram mins

