
-module(tut16).
-export([start/0, ping/1, pong/0, pong_v2/0]).

ping(0) ->
  pong ! finished,
  io:format("ping finished~n", []);

ping(N) ->
  pong ! {ping, self()},
  receive
    pong ->
      io:format("Ping received pong~n", [])
  end,
  ping(N -1).

pong() ->
  receive
    finished ->
      io:format("Pong finished~n", []);
    {ping, Ping_PID} ->
      io:format("Pong received ping ~w~n", [Ping_PID]),
      Ping_PID ! pong,
      pong()
  end.

pong_v2() ->
  receive
    finished ->
      io:format("Pong finished~n", []);
    {ping, Ping_PID} ->
      io:format("Pong received ping ~w~n", [Ping_PID]),
      ping_v2 ! pong,
      pong()
  end.

start() ->
  register(pong, spawn(tut16, pong, [])),
  register(ping_v2, spawn(tut16, ping, [3])).
%spawn(tut16, ping, [3]).

