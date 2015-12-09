<?php
/**
 * @file
 * Context Datalayer reaction plugin for Context API.
 */

/**
 * Context Reaction plugin to set datalayer values.
 */
class context_datalayer_reaction extends context_reaction {
  /**
   * Implements options_form().
   */
  public function options_form($context) {
    $defaults = $this->fetch_from_context($context, 'options');
    $form = parent::options_form($context);

    $form['overwrite'] = array(
      '#type' => 'checkbox',
      '#title' => t('Overwrite existing values'),
      '#description' => t('If a datalayer key is already set on a page, checking this option will allow you to overwrite it.'),
      '#default_value' => isset($defaults['overwrite']) ? $defaults['overwrite'] : FALSE,
    );

    $form['data'] = array(
      '#type' => 'container',
      '#prefix' => '<div id="datalayer-reaction-ajax-wrapper">',
      '#suffix' => '</div>',
      '#theme' => 'context_datalayer_form',
    );

    $form['data']['context_datalayer'] = array();
    if (!empty($context->reactions[$this->plugin]['data'])) {
      $form['data']['remove'] = array();
      foreach ($context->reactions[$this->plugin]['data'] as $key => $value) {
        $form['data']['context_datalayer'][$key] = array(
          '#type' => 'textfield',
          '#title' => t('Key'),
          '#title_display' => 'invisible',
          '#value' => $key,
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
            'callback' => 'context_datalayer_reaction_options_form_ajax',
            'wrapper' => 'datalayer-reaction-ajax-wrapper',
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
      '#description' => t('Provides this key-value pair to the datalayer when this section is active.'),
      '#size' => 16,
      '#maxlength' => 64,
      '#value' => '',
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
      '#value' => t('Add Reaction'),
      '#ajax' => array(
        'callback' => 'context_datalayer_reaction_options_form_ajax',
        'wrapper' => 'datalayer-reaction-ajax-wrapper',
      ),
      '#limit_validation_errors' => TRUE,
    );

    return $form;
  }

  /**
   * Options form validation handler.
   */
  public function options_form_validate($form, &$form_state) {
    // Context module doesn't allow for validation of reaction fields.
    // @todo: Raise bug for context module.
  }

  /**
   * Options form submit handler.
   */
  public function options_form_submit($values) {
    $data = array();

    // Build an array of key-value pairs.
    if (!empty($values['data']['context_datalayer'])) {
      foreach ($values['data']['context_datalayer'] as $key) {
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
        case t('Add Reaction'):
          $key = $values['data']['add']['key'];
          $value = $values['data']['add']['value'];
          // @todo: Move this to the validation callback.
          if (empty($key)) {
            form_set_error('', t('Key is a required field.'));
            break;
          }
          if (empty($value)) {
            form_set_error('', t('Value is a required field.'));
            break;
          }
          if (!empty($data[$key])) {
            form_set_error('', t('Key "%key" already exists.', array('%key' => $key)));
            break;
          }

          $data[$key] = $value;
          break;

        case t('Remove'):
          unset($data[$form_state['triggering_element']['#name']]);
          break;
      }
    }

    ksort($data);
    return array(
      'overwrite' => $values['overwrite'],
      'data' => $data,
    );
  }

  /**
   * Execute.
   */
  public function execute(&$datalayer) {
    foreach ($this->get_contexts() as $context) {
      $options = $this->fetch_from_context($context);
      if (!empty($options['data'])) {
        if ($options['overwrite']) {
          $datalayer = array_merge($datalayer, $options['data']);
        }
        else {
          $datalayer += $options['data'];
        }
      }
    }
  }
}

/**
 * AJAX callback for Context reaction options form.
 */
function context_datalayer_reaction_options_form_ajax($form, $form_state) {
  return $form['reactions']['plugins']['context_datalayer_reaction']['data'];
}
