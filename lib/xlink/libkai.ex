defmodule LibKai do
  require Logger
  use GenServer

  def start_link do
    GenServer.start_link(__MODULE__, [], [name: __MODULE__])
  end

  def init([]) do
    exe = Path.join(:code.priv_dir(:xlink), "libkai_port")
    opts = [
      :exit_status,
      :binary,
      # :stderr_to_stdout,
      {:args, []},
    ]
    port = Port.open({:spawn_executable, exe}, opts)
    {:ok, %{port: port}}
  end

  def handle_info({_port, {:exit_status, 0}}, state) do
    {:stop, :normal, state}
  end

  def handle_info({_port, {:exit_status, reason}}, state) do
    {:stop, {:port_exit, reason}, state}
  end

  def handle_info(info, state) do
    Logger.error "unhandled info:  #{inspect info}"
    {:noreply, state}
  end
end
