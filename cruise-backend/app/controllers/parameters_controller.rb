class ParametersController < ApplicationController
  skip_before_action :verify_authenticity_token
  before_action :set_parameter, only: %i[show update destroy]

  # GET /parameterrs or /parameters.json
  def index
    @parameters = Parameter.all
    render json: @parameters
  end

  # GET /parameters/1 or /parameters/1.json
  def show
    render json: @parameter
  end

  # POST /parameters or /parameters.json
  def create
    @parameter = Parameter.new(parameter_params)
    if @parameter.save
      render json: @parameter, status: :created
    else
      render json: { errors: @parameter.errors.full_messages }, status: :unprocessable_entity
    end
  end

  # PATCH/PUT /parameters/1 or /parameters/1.json
  def update
    if @parameter.update(parameter_params)
      render json: @parameter, status: :ok
    else
      render json: { errors: @parameter.errors.full_messages }, status: :unprocessable_entity
    end
  end

  # DELETE /parameters/1 or /parameters/1.json
  def destroy
    @parameter.destroy
    render json: { message: "Parameter successfully destroyed" }, status: :ok
  end

  private
  # Use callbacks to share common setup or constraints between actions.
  def set_parameter
    @parameter = Parameter.find(params[:id])
  end

  # Only allow a list of trusted parameters through.
  def parameter_params
    params.permit(:speed, :distance)
  end
end
