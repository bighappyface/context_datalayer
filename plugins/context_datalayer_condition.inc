<?php
/**
 * @file
 * Context Datalayer condition plugin for Context API.
 */

/**
 * Context Condition plugin to detect datalayer values.
 */
class context_datalayer_condition extends context_condition {
  /**
   * Implements condition_form().
   */
  public function condition_form($context) {
    $form['data'] = array(
      '#type' => 'container',
      '#prefix' => '<div id="datalayer-condition-ajax-wrapper">',
      '#suffix' => '</div>',
      '#theme' => 'context_datalayer_form',
    );

    $form['data']['context_datalayer'] = array();
    if (!empty($context->conditions[$this->plugin]['values'])) {
      $form['data']['remove'] = array();
      foreach ($context->conditions[$this->plugin]['values'] as $key => $value) {
        $form['data']['context_datalayer'][$key] = array(
          '#type' => 'textfield',
          '#title' => t('Key'),
          '#title_display' => 'invisible',
          '#size' => 16,
          '#maxlength' => 64,
          '#value' => $key,
        );
        $form['data']['types'][$key] = array(
          '#type' => 'select',
          '#title' => t('Type'),
          '#title_display' => 'invisible',
          '#options' => array(
            'string' => t('String'),
            'integer' => t('Integer'),
            'boolean' => t('Boolean'),
          ),
          '#value' => gettype($value),
        );
        $form['data']['values'][$key] = array(
          '#type' => 'textfield',
          '#title' => t('Value'),
          '#title_display' => 'invisible',
          '#value' => $value,
        );
        $form['data']['remove'][$key] = array(
          '#type' => 'button',
          '#value' => t('Remove'),
          '#name' => $key,
          '#ajax' => array(
            'callback' => 'context_datalayer_condition_options_form_ajax',
            'wrapper' => 'datalayer-condition-ajax-wrapper',
          ),
          '#limit_validation_errors' => TRUE,
        );
      }
    }

    // Add a new KVP.
    $form['data']['add'] = array(
      '#type' => 'container',
    );
    $form['data']['add']['key'] = array(
      '#type' => 'textfield',
      '#title' => t('Add Key/ Value'),
      '#description' => t('Provides this key-value pair to the datalayer when this section is active. Please use 0/1 for representing booleans.'),
      '#size' => 16,
      '#maxlength' => 64,
      '#value' => '',
    );
    $form['data']['add']['type'] = array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#title_display' => 'invisible',
      '#options' => array(
        'string' => t('String'),
        'integer' => t('Integer'),
        'boolean' => t('Boolean'),
      ),
      '#value' => 'string',
    );
    $form['data']['add']['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Add Value'),
      '#title_display' => 'invisible',
      '#maxlength' => 64,
      '#value' => '',
    );
    $form['data']['add']['button'] = array(
      '#type' => 'button',
      '#value' => t('Add Condition'),
      '#ajax' => array(
        'callback' => 'context_datalayer_condition_options_form_ajax',
        'wrapper' => 'datalayer-condition-ajax-wrapper',
      ),
      '#limit_validation_errors' => TRUE,
    );

    return $form;
  }

  /**
   * Conditions form validation handler.
   */
  public function condition_form_validate($form, &$form_state) {
    // Context module doesn't allow for validation of conditions fields.
    // @todo: Raise bug for context module.
  }

  /**
   * Conditions form submit handler.
   */
  public function condition_form_submit($values) {
    $data = array();

    // Build an array of key-value pairs.
    if (!empty($values['data']['context_datalayer'])) {
      foreach ($values['data']['context_datalayer'] as $key) {
        settype($values['data']['values'][$key], $values['data']['types'][$key]);
        $data[$key] = $values['data']['values'][$key];
      }
    }

    // AJAX handler for Add and Remove buttons.
    if (strstr(request_uri(), 'system/ajax')) {
      $form_state = array('submitted' => FALSE);
      $form_build_id = $_POST['form_build_id'];
      $form = form_get_cache($form_build_id, $form_state);

      $form_state['input'] = $_POST;
      $form_state['values'] = array();
      $form = form_builder($form['#form_id'], $form, $form_state);

      switch ($form_state['triggering_element']['#value']) {
        case t('Add Condition'):
          $key = $values['data']['add']['key'];
          $type = $values['data']['add']['type'];
          $value = $values['data']['add']['value'];
          // @todo: Move this to the validation callback.
          if (empty($key)) {
            form_set_error('', t('Key is a required field.'));
            break;
          }
          if (empty($value) && $value !== '0') {
            form_set_error('', t('Value is a required field.'));
            break;
          }
          if (!empty($data[$key])) {
            form_set_error('', t('Key "%key" already exists.', array('%key' => $key)));
            break;
          }
          if ($type == 'boolean' && !in_array($value, array('0', '1'))) {
            form_set_error('', t('Value "%value" should be either 0 or 1 when set to the boolean type.', array('%value' => $value)));
            break;
          }
          if ($type == 'number' && !is_numeric($value)) {
            form_set_error('', t('Value "%value" is not numeric.', array('%value' => $value)));
            break;
          }

          settype($value, $type);
          $data[$key] = $value;
          break;

        case t('Remove'):
          unset($data[$form_state['triggering_element']['#name']]);
          break;
      }
    }

    ksort($data);
    return $data;
  }

  /**
   * Execute.
   */
  public function execute($datalayer) {
    if ($this->condition_used()) {
      foreach ($this->get_contexts() as $context) {
        foreach ($this->fetch_from_context($context, 'values') as $key => $value) {
          if (isset($datalayer[$key]) && $datalayer[$key] = $value) {
            $this->condition_met($context);
          }
        }
      }
    }
  }
}

/**
 * AJAX callback for Context reaction options form.
 */
function context_datalayer_condition_options_form_ajax($form, $form_state) {
  return $form['conditions']['plugins']['context_datalayer_condition']['values']['data'];
}
