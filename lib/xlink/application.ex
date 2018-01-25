defmodule Xlink.Application do
  use Application
  alias Xlink.Task
  import Supervisor.Spec, warn: false

  def start(_type, _args) do
    children = [
      network_config(),

      worker(Task, [
        Xlink.Ntp, :set_time, []
      ], [id: :ntp]),

      worker(Task, [
        Xlink.Downloder, :dl_and_unarchive, []
      ], [restart: :transient, id: :downloader]),

      worker(Xlink.Engine, [], [])
    ] |> List.flatten()

    opts = [strategy: :one_for_one, name: Xlink.Supervisor]
    Supervisor.start_link(children, opts)
  end

  def network_config do
    conf = get_config().network
    Enum.map(conf, fn({iface, settings}) ->
      [
        worker(Xlink.NetworkUp, [iface, settings]),
        worker(Task, [
          Xlink.NetworkUp, :wait_for_dns, [iface]
        ], [id: :"network_up_#{iface}"])
      ]
    end) |> List.flatten()
  end

  def get_config do
    File.read!(Application.get_env(:xlink, :config_file))
    |> parse_config()
  end

  def parse_config(bin, state \\ %{in_comment: false, field: nil, field_acc: nil}, acc \\ %{network: %{}})

  def parse_config(<<"#", rest :: binary>>, state, acc) do
    parse_config(rest, %{state | in_comment: true}, acc)
  end

  def parse_config(<<"\n", rest :: binary >>, %{in_comment: true} = state, acc) do
    parse_config(rest, %{state | in_comment: false}, acc)
  end

  def parse_config(<<_, rest :: binary>>, %{in_comment: true} = state, acc) do
    parse_config(rest, state, acc)
  end

  def parse_config(<<"interface ", rest :: binary>>, state, acc) do
    parse_config(rest, %{state | field: :interface, field_acc: <<>>}, acc)
  end

  def parse_config(<<"\n", rest :: binary>>, %{field: :interface, field_acc: field_acc} = state, acc) do
    parse_config(rest, %{state | field: nil, field_acc: nil}, parse_interface_config(acc, field_acc))
  end

  def parse_config(<<char, rest :: binary>>, %{field: :interface, field_acc: field_acc} = state, acc) do
    parse_config(rest, %{state | field_acc: field_acc <> <<char>>}, acc)
  end

  def parse_config(<<"\n", rest :: binary>>, state, acc) do
    parse_config(rest, state, acc)
  end

  def parse_config(<<"">>, _state, acc), do: acc

  def parse_config(<<"", rest :: binary>>, state, acc) do
    parse_config(rest, state, acc)
  end

  def parse_config(<<>>, _state, acc), do: acc

  def parse_interface_config(acc, config) do
    [interface | configs] = String.split(String.trim(config), ", ")
    network_config = Map.new(configs, fn(str) ->
      [key, val] = String.split(str, "=")
      {String.to_atom(key), val}
    end)
    val = if Enum.empty?(network_config), do: [], else: network_config
    network = Map.put(acc.network, interface, val)
    %{acc | network: network}
  end
end
